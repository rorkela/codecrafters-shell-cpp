#include "tokenize.h"

enum class token_state {
  NORMAL,
  DOUBLE_Q,
  Q,
};
std::vector<std::string> tokenize(const std::string_view args) {
  std::vector<std::string> tokens;
  std::string current_token;
  token_state state = token_state::NORMAL;
  current_token = "";
  bool is_escape = false;
  for (char c : args) {
    switch (state) {
    case token_state::NORMAL: {
      if (is_escape) {
        current_token += c;
        is_escape = false;
      } else {
        if (c == ' ' || c == '\t') {
          if (!current_token.empty()) {
            tokens.emplace_back(current_token);
            current_token = "";
          }
        } else if (c == '\\') {
          is_escape = true;
        } else if (c == '\'') {
          state = token_state::Q;
        } else if (c == '\"') {
          state = token_state::DOUBLE_Q;
        } else {
          current_token += c;
        }
      }
      break;
    }
    case token_state::Q: {
      if (c == '\'')
        state = token_state::NORMAL;
      else
        current_token += c;
      break;
    }
    case token_state::DOUBLE_Q: {
      if (is_escape) {
        current_token += c;
        is_escape = false;
      } else {
        if (c == '\\') {
          is_escape = true;
        } else if (c == '\"') {
          state = token_state::NORMAL;
        } else {
          current_token += c;
        }
      }
      break;
    }
    }
  }
    if (!current_token.empty())
      tokens.emplace_back(current_token);
  return tokens;
}
