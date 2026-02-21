#include <bits/stdc++.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>
enum class Builtin { EXIT, ECHO, TYPE, PWD, CD};
#include "helper.h"
#include "tokenize.h"
int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string path_env = std::getenv("PATH");
  std::vector<std::string> path_dirs;
  size_t start = 0;
  size_t end = path_env.find(':');
  while (end != std::string::npos) {
    path_dirs.push_back(path_env.substr(start, end - start));
    start = end + 1;
    end = path_env.find(':', start);
  }
  path_dirs.push_back(path_env.substr(start)); // Add the last directory
  std::unordered_map<std::string, Builtin> builtins = {
      {"exit", Builtin::EXIT}, {"echo", Builtin::ECHO}, {"type", Builtin::TYPE},
      {"pwd", Builtin::PWD},   {"cd", Builtin::CD}     };

  while (1) {
    std::cout << "$ ";
    std::string cmd;
    std::getline(std::cin, cmd);
    std::vector<std::string> argv = tokenize(cmd);
    // Builtin Command
    auto it = builtins.find(argv[0]);
    if (it != builtins.end()) {
      switch (it->second) {
      case Builtin::EXIT: {
        return 0;
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
            std::cout << argv[1] << ": not found" << std::endl;
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
        if (argv[1][0] == '~') {
          std::string home_dir = std::getenv("HOME");
          argv[1].replace(0, 1, home_dir);
        }
        if (chdir(argv[1].c_str()) != 0) {
          std::cout << "cd: " << argv[1] << ": No such file or directory"
                    << std::endl;
        }
        break;
      }
      }
    } else {
      run_exec(argv,path_dirs);
      /*std::string full_path = find_executable(argv[0], path_dirs);
      if (full_path.empty()) {
        std::cout << argv[0] << ": command not found" << std::endl;
      } else {
        run_exec(full_path, cmd, arms);*/
      
    }
  }
}