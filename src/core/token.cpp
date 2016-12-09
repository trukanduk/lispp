#include <lispp/token.h>

#include <map>

namespace lispp {

Token::Token(TokenType type) : type(type) {}

Token::Token(TokenType type, double number_value)
    : type(type), number_value(number_value) {}

Token::Token(TokenType type, const std::string& string_value)
    : type(type), string_value(string_value) {}


bool Token::operator==(const Token& other) const {
  return (type == other.type) && (number_value == other.number_value) &&
         (string_value == other.string_value);
}

bool Token::operator!=(const Token& other) const {
  return !(*this == other);
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
  static const std::map<TokenType, std::string> kTypeNames{
    {TokenType::kNumber,       "Number"},
    {TokenType::kCharacters,   "Character"},
    {TokenType::kSymbol,       "Symbol"},
    {TokenType::kComma,        "Comma"},
    {TokenType::kBackTick,     "BackTick"},
    {TokenType::kDot,          "Dot"},
    {TokenType::kQuote,        "Quote"},
    {TokenType::kOpenBracket,  "OpenBracket"},
    {TokenType::kCloseBracket, "CloseBracket"},
    {TokenType::kEndLine,      "EndLine"},
    {TokenType::kEnd,          "End"},
    {TokenType::kUndefined,    "Undefined"}
  };

  std::string type_name = "<unknown>";
  auto type_name_iter = kTypeNames.find(token.type);
  if (type_name_iter != kTypeNames.end()) {
    type_name = type_name_iter->second;
  }

  out << "(" << type_name;
  if (token.type == TokenType::kNumber) {
    out << " " << token.number_value;
  } else if (token.type == TokenType::kCharacters) {
    out << " \"" << token.string_value << "\"";
  } else if (token.type == TokenType::kSymbol) {
    out << " " << token.string_value;
  }
  out << ")";

  return out;
}

} // lispp
