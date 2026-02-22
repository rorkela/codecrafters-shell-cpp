#ifndef EXEC_H
#define EXEC_H

#include "tokenize.h"
#include <bits/stdc++.h>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <spawn.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
enum class Builtin { EXIT, ECHO, TYPE, PWD, CD };

extern std::unordered_map<std::string, Builtin> builtins;

struct Cmd {
  std::vector<std::string> argv;
  std::string out_write;
  std::string out_append;
  std::string err_write;
  std::string err_append;
  std::string inp;
};

void exec_full(std::vector<Token> tokens);
void run_cmd(const Cmd &cmd);
#endif