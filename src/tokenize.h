#ifndef TOKENIZE
#define TOKENIZE

#include <bits/stdc++.h>
struct Token {
  std::string text;
  bool is_op;
};
std::vector<Token> tokenize(std::string_view cmd);

#endif
