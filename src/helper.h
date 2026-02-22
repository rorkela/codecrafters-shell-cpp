#ifndef HELPER_H
#define HELPER_H
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <spawn.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

std::string find_executable(const std::string &cmd,
                            const std::vector<std::string> &path_dirs);

std::vector<std::string> split(const std::string &s, char delim);
#endif