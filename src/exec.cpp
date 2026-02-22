#include "exec.h"
#include "helper.h"
extern std::vector<std::string> path_dirs;

void exec_full(std::vector<Token> tokens) {
  std::vector<Token> argv = tokens;
  std::vector<Cmd> pipeline;
  Cmd current_command = {{}, "", "", "", "", ""};
  for (int i = 0; i < tokens.size(); i++) {
    if (tokens[i].is_op == false) {
      current_command.argv.emplace_back(tokens[i].text);
    }
    // token is operator but not pipe. hence redirection
    else if (tokens[i].text != "|") {
      std::string op = tokens[i].text;
      std::string file = tokens[i + 1].text;
      if (op == ">" || op == "1>") {
        current_command.out_write = file;
      } else if (op == ">>" || op == "1>>") {
        current_command.out_append = file;
      } else if (op == "2>") {
        current_command.err_write = file;
      } else if (op == "2>>") {
        current_command.err_append = file;
      }
      i++;
    }
    //token is pipe
    else{
      pipeline.push_back(current_command);
      current_command={{}, "", "", "", "", ""};
    }
  }
  pipeline.push_back(current_command);

  for (int i = 0; i < pipeline.size(); i++) {
    run_cmd(pipeline[i]);
  }
}

void run_cmd(const Cmd &cmd) {
  std::vector<std::string> argv = cmd.argv;
  //If no input
  if (argv.empty())
    return;
  // Builtin Command
  auto it = builtins.find(argv[0]);
  if (it != builtins.end()) {

    // Redirection
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stderr = dup(STDERR_FILENO);
    int saved_stdin = dup(STDIN_FILENO);

    if (!cmd.out_write.empty()) {
      int fd = open(cmd.out_write.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
      dup2(fd, STDOUT_FILENO);
      close(fd);
    } else if (!cmd.out_append.empty()) {
      int fd =
          open(cmd.out_append.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
    if (!cmd.err_write.empty()) {
      int fd = open(cmd.err_write.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
      dup2(fd, STDERR_FILENO);
      close(fd);
    } else if (!cmd.err_append.empty()) {
      int fd =
          open(cmd.err_append.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
      dup2(fd, STDERR_FILENO);
      close(fd);
    }
    switch (it->second) {
    case Builtin::EXIT: {
      std::exit(0);
      break;
    }
    case Builtin::ECHO: {
      for (int i = 1; i < argv.size(); i++) {
        if (i != 1)
          std::cout << " ";
        std::cout << argv[i];
      }
      std::cout << std::endl;
      break;
    }
    case Builtin::TYPE: {
      if (builtins.find(argv[1]) != builtins.end()) {
        // If Builtin
        std::cout << argv[1] << " is a shell builtin" << std::endl;
      } else {
        // Search in PATH
        std::string full_path = find_executable(argv[1], path_dirs);
        if (full_path.empty()) {
          std::cerr << argv[1] << ": not found" << std::endl;
        } else {
          std::cout << argv[1] << " is " << full_path << std::endl;
        }
      }
    } break;
    case Builtin::PWD: {
      char *cwd = getcwd(nullptr, 0);
      std::cout << cwd << std::endl;
      free(cwd);
      break;
    }
    case Builtin::CD: {
      if (argv.size() < 2) {
        chdir(std::getenv("HOME"));
      } else {
        if (argv[1][0] == '~') {
          std::string home_dir = std::getenv("HOME");
          argv[1].replace(0, 1, home_dir);
        }
        if (chdir(argv[1].c_str()) != 0) {
          std::cerr << "cd: " << argv[1] << ": No such file or directory"
                    << std::endl;
        }
      }
      break;
    }
    }
    // Redirection Close
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
  } else {
    // Redirection setup
    posix_spawn_file_actions_t actions;
    posix_spawn_file_actions_init(&actions);
    if (!cmd.out_write.empty()) {
      posix_spawn_file_actions_addopen(&actions, STDOUT_FILENO,
                                       cmd.out_write.c_str(),
                                       O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else if (!cmd.out_append.empty()) {
      posix_spawn_file_actions_addopen(&actions, STDOUT_FILENO,
                                       cmd.out_append.c_str(),
                                       O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    if (!cmd.err_write.empty()) {
    posix_spawn_file_actions_addopen(&actions,
                                     STDERR_FILENO,
                                     cmd.err_write.c_str(),
                                     O_WRONLY | O_CREAT | O_TRUNC,
                                     0644);
}
else if (!cmd.err_append.empty()) {
    posix_spawn_file_actions_addopen(&actions,
                                     STDERR_FILENO,
                                     cmd.err_append.c_str(),
                                     O_WRONLY | O_CREAT | O_APPEND,
                                     0644);
}
    pid_t pid;
    std::vector<std::string> args = cmd.argv;
    std::vector<char *> c_argv;
    for (const std::string &arg : args) {
      c_argv.push_back(const_cast<char *>(arg.c_str()));
    }
    c_argv.push_back(nullptr); // Null-terminate the arguments
    if (posix_spawnp(&pid, args[0].c_str(), &actions, nullptr, c_argv.data(),
                     environ) != 0) {
      std::cerr << args[0] << ": command not found" << std::endl;
      return;
    }

    waitpid(pid, nullptr, 0);
    posix_spawn_file_actions_destroy(&actions);
  }
}
