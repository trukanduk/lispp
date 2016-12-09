#include <lispp/istream_tokenizer.h>

#include <map>
#include <unordered_map>

namespace lispp {

IstreamTokenizer::IstreamTokenizer(std::istream& input) {
  set_input_stream(input);
}

IstreamTokenizer::~IstreamTokenizer() {}

Token IstreamTokenizer::next_token() {
  check_input_stream();

  peek_token();
  current_token_ = next_token_;
  next_token_ = Token();
  return current_token_;
}

Token IstreamTokenizer::peek_token() {
  check_input_stream();

  if (next_token_.type == TokenType::kUndefined ||
      next_token_.type == TokenType::kEnd) {
    next_token_ = parse_token();
  }

  return next_token_;
}

Token IstreamTokenizer::current_token() {
  return current_token_;
}

bool IstreamTokenizer::has_more_tokens() {
  skip_whitespaces(false);
  return input_->good();
}

int IstreamTokenizer::get_current_line() {
  return current_line_;
}

void IstreamTokenizer::clear() {}

void IstreamTokenizer::set_input_stream(std::istream& input) {
  // NOTE: Allow change input on runtime? Clear current token?
  //       Too much questions...
  input_ = &input;

  next_token_ = Token();
  // current_token_ = Token();
}

void IstreamTokenizer::check_input_stream() {
  if (input_ == nullptr) {
    throw std::runtime_error("Input stream is not set!");
  }
}

Token IstreamTokenizer::parse_token() {
  skip_whitespaces(false);

  const int current_char = input_->peek();
  if (!input_->good()) {
    return Token(TokenType::kEnd);
  } else if (current_char == '\n') {
    input_->get();
    ++current_line_;
    return Token(TokenType::kEndLine);
  } else if (current_char == '"') {
    return parse_characters_token();
  } else if (is_symbol_token_start(current_char)) {
    return parse_symbol_token();
  } else if (IsDigitExt(current_char)) {
    return parse_number_token();
  } else {
    return parse_one_symbol_token();
  }
}

Token IstreamTokenizer::parse_characters_token() {
  expect_char('"');
  std::string token_value = read_while(
      IsNotQuotes, "Unexpected end of file on reading string");
  expect_char('"');

  return Token(TokenType::kCharacters, token_value);
}

bool IstreamTokenizer::is_symbol_token_start(char current_char) {
  return IsInitialOfSymbol(current_char) ||
      (IsSign(current_char) && !IsDigitExt(peek_next_char()));
}

Token IstreamTokenizer::parse_symbol_token() {
  std::string value = read_while(IsSymbolChar);

  if (IsSign(value[0]) && value.size() != 1) {
    throw TokenizerError("Invalid identifier token '" + value + "'");
  }

  return Token(TokenType::kSymbol, value);
}

Token IstreamTokenizer::parse_number_token() {
  std::string string_value = read_while(IsDigitExt);

  if (string_value.find('+', 1) != std::string::npos ||
      string_value.find('-', 1) != std::string::npos) {
    throw TokenizerError("Invalid number token '" + string_value + "'");
  }

  if (string_value == ".") {
    return Token(TokenType::kDot);
  }

  const auto dotpos = string_value.find('.');
  if (string_value.find('.', dotpos + 1) == std::string::npos) {
    return Token(TokenType::kNumber, std::stod(string_value));
  }

  throw TokenizerError("Invalid number token '" + string_value + "'");
}

Token IstreamTokenizer::parse_one_symbol_token() {
  const auto current_char = input_->get();

  static const std::unordered_map<char, TokenType> kOneSymbolTokens{
    {',',  TokenType::kComma},
    {'`',  TokenType::kBackTick},
    {'.',  TokenType::kDot},
    {'\'', TokenType::kQuote},
    {'(',  TokenType::kOpenBracket},
    {')',  TokenType::kCloseBracket}
  };

  const auto one_symbol_token_iter = kOneSymbolTokens.find(current_char);
  if (one_symbol_token_iter != kOneSymbolTokens.end()) {
    return Token(one_symbol_token_iter->second);
  }

  throw TokenizerError("Unexpected symbol: "
                       "'" + std::string(1, current_char) + "'" +
                       " (" + std::to_string(int(current_char)) + ")");
}

void IstreamTokenizer::skip_whitespaces(bool with_eol) {
  while (input_->good() && IsWhiteSpace(input_->peek(), with_eol)) {
    input_->get();
  }
}

void IstreamTokenizer::expect_char(char c, bool extract) {
  if (input_->peek() != c) {
    throw TokenizerError("Expected '" + std::string(1, c)
                         + "' got '" + std::string(1, input_->peek()) + "'");
  }

  if (extract) {
    input_->get();
  }
}

int IstreamTokenizer::peek_next_char() {
  input_->get();
  const int next_char = input_->peek();
  input_->unget();

  return next_char;
}

} // lispp
