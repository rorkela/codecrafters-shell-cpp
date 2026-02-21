#ifndef HELPER_H
#define HELPER_H
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <spawn.h>
#include <sys/wait.h>
std::string find_executable(const std::string &cmd, const std::vector<std::string> &path_dirs);
void run_exec(const std::string &full_path, const std::string &cmd_args);
std::vector<std::string> split(const std::string& s, char delim);
#endif