#pragma once

#include <iostream>
#include <string>

namespace lispp {

enum class TokenType {
  kNumber,       // +123 or -.4
  kCharacters,   // "foo"
  kSymbol,       // foo-bar
  kComma,        // ,
  kBackTick,     // `
  // kNumQuote,     // #' -- Pure lip only!?
  kDot,          // .
  kQuote,        // '
  kOpenBracket,  // (
  kCloseBracket, // )
  kEndLine,      // \n
  kEnd,          // <end of file>

  kUndefined
};

struct Token final {
  Token() = default;
  explicit Token(TokenType type);
  Token(TokenType type, double number_value);
  Token(TokenType type, const std::string& string_value);

  bool operator==(const Token& other) const;
  bool operator!=(const Token& other) const;

  TokenType type = TokenType::kUndefined;

  bool bool_value = false;
  double number_value = 0.0;
  std::string string_value;
};

std::ostream& operator<<(std::ostream& out, const Token& token);

} // lispp
