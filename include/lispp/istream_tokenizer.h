#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include <cctype>

#include <lispp/token.h>
#include <lispp/tokenizer.h>

namespace lispp {

// TODO: file_tokenizer
class IstreamTokenizer : public ITokenizer {
public:
  IstreamTokenizer() = default;
  explicit IstreamTokenizer(std::istream& input);
  ~IstreamTokenizer() override;

  Token next_token() override;
  Token current_token() override;
  Token peek_token() override;
  bool has_more_tokens() override;

  int get_current_line() override;
  void clear() override;

protected:
  // NOTE: call this from subclasses in constructors.
  void set_input_stream(std::istream& input);
  void check_input_stream();

private:
  Token parse_token();
  Token parse_characters_token();
  bool is_symbol_token_start(char current_char);
  Token parse_symbol_token();
  Token parse_number_token();
  Token parse_one_symbol_token();

  template<typename Condition>
  std::string read_while(Condition condition,
                        const std::string& error_message) {
    std::string result;

    while (input_->good() && condition(input_->peek())) {
      result.push_back(input_->get());
    }

    if (!input_->good() && !error_message.empty()) {
      throw TokenizerError(error_message);
    }

    return result;
  }

  template<typename Condition>
  std::string read_while(Condition condition) {
    return read_while(condition, "");
  }

  static bool IsWhiteSpace(char c, bool accept_eol = false) {
    return std::isspace(c) && (accept_eol || c != '\n');
  }

  static bool IsDigit(char c) {
    return std::isdigit(c);
  }

  static bool IsSign(char c) {
    return (c == '-') || (c == '+');
  }

  static bool IsDigitExt(char c) {
    return IsDigit(c) || IsSign(c) || (c == '.');
  }

  static bool IsNotQuotes(char c) {
    return c != '"';
  }

  static bool IsInitialOfSymbol(char c) {
    static const std::unordered_set<char> kValidSymbols{
      '!', '$', '%', '&', '*', '/', ':', '<', '=', '>', '?', '~', '_', '^', '#'
    };

    return std::isalpha(c) || (kValidSymbols.count(c) > 0);
  }

  static bool IsSymbolChar(char c) {
    return IsInitialOfSymbol(c) || std::isdigit(c) ||
           c == '.' || c == '-' || c == '+';
  }

  void skip_whitespaces(bool with_eol = false);
  void expect_char(char c, bool extract = true);
  int peek_next_char();


  std::istream* input_ = nullptr;
  Token next_token_;
  Token current_token_;
  int current_line_ = 0;
};

} // lispp
