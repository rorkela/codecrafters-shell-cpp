#include "helper.h"
std::string find_executable(const std::string &cmd,
                            const std::vector<std::string> &path_dirs) {
  for (const auto &dir : path_dirs) {
    std::string full_path = dir + "/" + cmd;
    if (!access(full_path.c_str(), X_OK)) {
      return full_path;
    }
  }
  return "";
}

// Run_exec function using posix_spawn

extern char **environ;

// TODO: args and argv are ambigious for now. FIx LATER

// TODO: Implement multiple spaces in cmd_arg
std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> tokens;
  size_t start = 0, end;

  while ((end = s.find(delim, start)) != std::string::npos) {
    tokens.emplace_back(s.substr(start, end - start));
    start = end + 1;
  }

  tokens.emplace_back(s.substr(start));
  return tokens;
}