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
enum class Builtin { EXIT, ECHO, TYPE, PWD, CD, NOT_BUILTIN };

extern std::unordered_map<std::string, Builtin> builtins;

struct Cmd {
  std::vector<std::string> argv;
  std::string out_write;
  std::string out_append;
  std::string err_write;
  std::string err_append;
  std::string inp;
  Builtin cmd_type;
  bool fork;
  int pipein;
  int pipeout;
  bool ispipein;
  bool ispipeout;
};

void exec_full(std::vector<Token> tokens);
void exec_cmd(const Cmd &cmd);
void run_cmd(const Cmd &cmd);
void redirect(const Cmd &cmd);
#endif