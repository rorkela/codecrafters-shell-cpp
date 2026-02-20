#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::unordered_set<std::string> builtins={"exit", "echo", "type"};

  while (1) {
    std::cout << "$ ";
    std::string cmd;
    std::getline(std::cin, cmd);
    int cmd_sep = cmd.find(' ');
    std::string cmd_name = cmd.substr(0, cmd_sep);
    std::string cmd_args = cmd.substr(cmd_sep + 1);
    if (cmd_name == "exit") {
      break;
    } else if (cmd_name == "echo") {
      std::cout << cmd_args << std::endl;
    } else if (cmd_name == "type"){
      if(builtins.find(cmd_args) != builtins.end()){
      std::cout << cmd_args << " is a shell builtin" << std::endl;}
      else{
        std::cout << cmd_args << ": not found" << std::endl;
      }
    } else {
      std::cout << cmd << ": command not found" << std::endl;
    }
  }
}
