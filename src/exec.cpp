#include "exec.h"
#include "helper.h"
extern std::vector<std::string> path_dirs;

std::vector <pid_t> pids;
void exec_full(std::vector<Token> tokens) {
  pids.clear();
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
    // token is pipe
    else {
      pipeline.push_back(current_command);
      current_command = {{}, "", "", "", "", ""};
    }
  }
  pipeline.push_back(current_command);
  int pipefd[2];

  // Piping
  pipeline[0].ispipein=false;
    pipeline[pipeline.size()-1].ispipeout=false;
  for (int i = 0; i < pipeline.size(); i++) {
  if (pipeline.size() == 1) {
      pipeline[i].ispipein = false;
      pipeline[i].ispipeout = false;
    } else {
      if (i !=pipeline.size()-1) {
        pipe(pipefd);
        pipeline[i + 1].pipein = pipefd[0];
        pipeline[i].pipeout = pipefd[1];
        pipeline[i+1].ispipein=true;
        pipeline[i].ispipeout=true;
      }
    }
  }
  for (int i = 0; i < pipeline.size(); i++) {
    Cmd &cmd = pipeline[i];

    // Execution handling
    auto it = builtins.find(cmd.argv[0]);
    if (it != builtins.end()) {
      cmd.cmd_type = it->second;
    } else {
      cmd.cmd_type = Builtin::NOT_BUILTIN;
    }
    if (cmd.cmd_type == Builtin::NOT_BUILTIN || pipeline.size() != 1) {
      cmd.fork = 1;
    }
    // Redirection
    exec_cmd(cmd);
  }
  for(pid_t p:pids){
    waitpid(p, nullptr, 0);
  }
  
}

void exec_cmd(const Cmd &cmd) {
  pid_t pid = 1;
  if (cmd.fork) {
    pid = fork();
  }
  // If its in parent and not forked. or its in child and forked
  if ((pid > 0 && cmd.fork == false) || (pid == 0 && cmd.fork == true)) {
    // Redirection

    run_cmd(cmd);

    if (cmd.fork == true) {
      _exit(0);
    }
  }
  if (pid < 0 && cmd.fork == true) {
    std::cerr << "Fork Fail" << std::endl;
  }
  // if in parent when forked.
  else if (pid > 0 && cmd.fork == true) {
    if (cmd.ispipein)
        close(cmd.pipein);

    if (cmd.ispipeout)
        close(cmd.pipeout);
    pids.push_back(pid);
  }
}

void redirect(const Cmd &cmd) {

  // pipeline redirection
  if (cmd.ispipein) {
    dup2(cmd.pipein, STDIN_FILENO);
    close(cmd.pipein);
  }
  if (cmd.ispipeout){
    dup2(cmd.pipeout, STDOUT_FILENO);
    close(cmd.pipeout);
  }
  if (!cmd.out_write.empty()) {
    int fd = open(cmd.out_write.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDOUT_FILENO);
    close(fd);
  } else if (!cmd.out_append.empty()) {
    int fd = open(cmd.out_append.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    dup2(fd, STDOUT_FILENO);
    close(fd);
  }
  if (!cmd.err_write.empty()) {
    int fd = open(cmd.err_write.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDERR_FILENO);
    close(fd);
  } else if (!cmd.err_append.empty()) {
    int fd = open(cmd.err_append.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    dup2(fd, STDERR_FILENO);
    close(fd);
  }
}

void run_cmd(const Cmd &cmd) {
  std::vector<std::string> argv = cmd.argv;
  int saved_stdout = dup(STDOUT_FILENO);
  int saved_stderr = dup(STDERR_FILENO);
  int saved_stdin = dup(STDIN_FILENO);
  redirect(cmd);
  // If no input
  if (argv.empty())
    return;

  // Builtin Command

  if (cmd.cmd_type != Builtin::NOT_BUILTIN) {

    switch (cmd.cmd_type) {
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

  } else {

    std::vector<std::string> args = cmd.argv;
    std::vector<char *> c_argv;
    for (const std::string &arg : args) {
      c_argv.push_back(const_cast<char *>(arg.c_str()));
    }
    c_argv.push_back(nullptr); // Null-terminate the arguments

    execvp(c_argv[0], c_argv.data());
    std::cerr << c_argv[0] << ": command not found" << std::endl;
    std::exit(0);
  }
  // Redirection Close
  if (cmd.fork == false) {
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stderr);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
  }
}
