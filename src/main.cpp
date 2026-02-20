#include <iostream>
#include <string>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // TODO: Uncomment the code below to pass the first stage
  std::cout << "$ ";
  std::string cmd_name;
  std::getline(std::cin, cmd_name);
  std::cout<< cmd_name <<": command not found"<<std::endl;
}
