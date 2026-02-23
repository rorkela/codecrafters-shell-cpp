#include "tokenize.h"

enum class token_state {
  NORMAL,
  DOUBLE_Q,
  Q,
};
std::vector<Token> tokenize(const std::string_view cmd) {
  std::vector<Token> tokens;
  Token current_token;
  token_state state = token_state::NORMAL;
  current_token.text = "";
  current_token.is_op = false;
  bool is_escape = false;

  char c;
  for (int i = 0; i < cmd.length(); i++) {
    c = cmd[i];
    switch (state) {
    case token_state::NORMAL: {
      if (is_escape) {
        current_token.text += c;
        is_escape = false;
      } else {
        if (c == ' ' || c == '\t') {
          if (!current_token.text.empty()) {
            // Token insertion here
            tokens.emplace_back(current_token);
            current_token.text = "";
            current_token.is_op = false;
          }
        } else if (c == '\\') {
          is_escape = true;
        } else if (c == '\'') {
          state = token_state::Q;
        } else if (c == '\"') {
          state = token_state::DOUBLE_Q;
        } else if (c == '|') {
          if (!current_token.text.empty()) {
            tokens.emplace_back(current_token);
            current_token.text = "";
            current_token.is_op = false;
          }
          current_token.text = "|";
          current_token.is_op = true;
          tokens.emplace_back(current_token);
          current_token.text = "";
          current_token.is_op = false;
        } else {
          // Operator check and token insertion
          int op_size = 0; // if stays 0, no op detected
          std::string mask1 = std::string(cmd.substr(i, 1));
          std::string mask2 = std::string(cmd.substr(i, 2));
          std::string mask3 = std::string(cmd.substr(i, 3));
          std::string op_token = "";
          if (mask3 == "1>>" || mask3 == "2>>") {
            op_size = 3;
            op_token = mask3;
          } else if (mask2 == "1>" || mask2 == "2>" || mask2 == ">>") {
            op_size = 2;
            op_token = mask2;
          } else if (mask1 == ">") {
            op_size = 1;
            op_token = mask1;
          }
          if (op_size == 0) {
            current_token.text += c;
          } else {
            i += op_size - 1;
            if (!current_token.text.empty()) {
              tokens.emplace_back(current_token);
              current_token.text = "";
              current_token.is_op = false;
            }
            current_token.text = op_token;
            current_token.is_op = true;
            tokens.emplace_back(current_token);
            current_token.text = "";
            current_token.is_op = false;
          }
        }
      }
      break;
    }
    case token_state::Q: {
      if (c == '\'')
        state = token_state::NORMAL;
      else
        current_token.text += c;
      break;
    }
    case token_state::DOUBLE_Q: {
      if (is_escape) {
        current_token.text += c;
        is_escape = false;
      } else {
        if (c == '\\') {
          is_escape = true;
        } else if (c == '\"') {
          state = token_state::NORMAL;
        } else {
          current_token.text += c;
        }
      }
      break;
    }
    }
  }
  if (!current_token.text.empty())
    tokens.emplace_back(current_token);
  return tokens;
}
