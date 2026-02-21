#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
enum class Builtin { EXIT, ECHO, TYPE };
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
  std::unordered_map<std::string, Builtin> builtins = {{"exit", Builtin::EXIT},
                                                       {"echo", Builtin::ECHO},
                                                       {"type", Builtin::TYPE}};

  while (1) {
    std::cout << "$ ";
    std::string cmd;
    std::getline(std::cin, cmd);
    int cmd_sep = cmd.find(' ');
    std::string cmd_name;
    std::string cmd_args;
    if (cmd_sep == std::string::npos) {
      cmd_name = cmd;
      cmd_args = "";
    } else {
      cmd_name = cmd.substr(0, cmd_sep);
      cmd_args = cmd.substr(cmd_sep + 1);
    }
    // Builtin Command
    auto it = builtins.find(cmd_name);
    if (it != builtins.end()) {
      switch (it->second) {
      case Builtin::EXIT:
        return 0;
        break;
      case Builtin::ECHO:
        std::cout << cmd_args << std::endl;
        break;
      case Builtin::TYPE:
        if (builtins.find(cmd_args) != builtins.end()) {
          // If Builtin
          std::cout << cmd_args << " is a shell builtin" << std::endl;
        } else {
          // Search in PATH
          bool found = false;
          for (const auto &dir : path_dirs) {
            std::string full_path = dir + "/" + cmd_args;
            //Check if exists
            if (std::ifstream(full_path).good() && ! access(full_path.c_str(), X_OK)) {
              std::cout << cmd_args << " is " << full_path << std::endl;
              found = true;
              break;
            }
          }
          if (!found) {
            std::cout << cmd_args << ": not found" << std::endl;
          }
        }
        break;
      }
    } else {
      std::cout << cmd << ": command not found" << std::endl;
    }
  }
}
