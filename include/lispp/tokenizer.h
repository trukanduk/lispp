#pragma once

#include <iostream>
#include <string>

#include <lispp/token.h>

namespace lispp {

class TokenizerError : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

class ITokenizer {
public:
  virtual ~ITokenizer() = 0;

  virtual Token next_token() = 0;
  virtual Token current_token() = 0;
  virtual Token peek_token() = 0;
  virtual bool has_more_tokens() = 0;

  virtual int get_current_line() = 0;
  virtual void clear() = 0;
};

} // lispp
