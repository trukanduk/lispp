#pragma once

#include <memory>

#include <lispp/object.h>
#include <lispp/tokenizer.h>
#include <lispp/object_ptr.h>

namespace lispp {

class ParserError : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

class Parser {
public:
  explicit Parser(ITokenizer* tokenizer);

  bool has_objects(bool ignore_endlines = true);
  ObjectPtr<> parse_object();

private:
  ObjectPtr<ConsObject> parse_begun_list();
  void expect_token(const Token& tok, bool extract = true);
  void skip_endlines();

  ITokenizer* tokenizer_;
};

} // lispp
