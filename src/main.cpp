#include <bits/stdc++.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unistd.h>

#include "exec.h"
#include "helper.h"
#include "tokenize.h"
std::unordered_map<std::string, Builtin> builtins = {{"exit", Builtin::EXIT},
                                                     {"echo", Builtin::ECHO},
                                                     {"type", Builtin::TYPE},
                                                     {"pwd", Builtin::PWD},
                                                     {"cd", Builtin::CD}};

std::vector<std::string> path_dirs;

int main() {

  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Getting Path Directories
  std::string path_env = std::getenv("PATH");

  size_t start = 0;
  size_t end = path_env.find(':');
  while (end != std::string::npos) {
    path_dirs.push_back(path_env.substr(start, end - start));
    start = end + 1;
    end = path_env.find(':', start);
  }
  path_dirs.push_back(path_env.substr(start)); // Add the last directory

  while (1) {
    std::cout << "$ ";
    std::string cmd;
    std::getline(std::cin, cmd);
    std::vector<Token> tokens = tokenize(cmd);
    exec_full(tokens);
  }
}