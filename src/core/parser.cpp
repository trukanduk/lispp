#include <lispp/parser.h>

#include <lispp/istream_tokenizer.h>
#include <lispp/objects_all.h>
#include <lispp/object_ptr.h>

namespace lispp {

Parser::Parser(ITokenizer* tokenizer)
    : tokenizer_(tokenizer) {}

bool Parser::has_objects(bool ignore_endlines) {
  if (ignore_endlines) {
    skip_endlines();
  }

  return (tokenizer_->peek_token().type != TokenType::kEnd) &&
         (tokenizer_->peek_token().type != TokenType::kEndLine);
}

ObjectPtr<> Parser::parse_object() {
  skip_endlines();
  const auto current_token = tokenizer_->next_token();

  if (current_token.type == TokenType::kNumber) {
    double value = current_token.number_value;
    return new NumberObject(value);

  } else if (current_token.type == TokenType::kCharacters) {
    std::string value = current_token.string_value;
    return new CharactersObject(value);

  } else if (current_token.type == TokenType::kSymbol) {
    std::string value = current_token.string_value;
    if (value == "#t") {
      return new BooleanObject(true);
    } else if (value == "#f") {
      return new BooleanObject(false);
    } else {
      return new SymbolObject(value);
    }

  } else if (current_token.type == TokenType::kQuote) {
    ObjectPtr<> underlying_object(parse_object());
    return new QuoteObject(underlying_object);

  } else if (current_token.type == TokenType::kComma) {
    ObjectPtr<> underlying_object(parse_object());
    return new CommaObject(underlying_object);

  } else if (current_token.type == TokenType::kBackTick) {
    ObjectPtr<> underlying_object(parse_object());
    return new BackTickObject(underlying_object);

  } else if (current_token.type == TokenType::kOpenBracket) {
    return parse_begun_list();

  } else if (current_token.type == TokenType::kEnd) {
    return nullptr;

  }

  std::stringstream ss;
  ss << "Unexpected token: " << current_token;
  throw ParserError(ss.str());
}

ObjectPtr<ConsObject> Parser::parse_begun_list() {
  skip_endlines();
  if (tokenizer_->peek_token().type == TokenType::kCloseBracket) {
    tokenizer_->next_token();
    return nullptr;
  }

  ObjectPtr<> left_object(parse_object());
  ObjectPtr<ConsObject> list_item(new ConsObject(left_object));
  Token current_token = tokenizer_->peek_token();

  if (current_token.type == TokenType::kDot) {
    tokenizer_->next_token();
    list_item->set_right_value(parse_object());
    expect_token(Token(TokenType::kCloseBracket), true);
  } else if (current_token.type == TokenType::kEnd) {
    throw ParserError("Unexpected end of file");
  } else {
    list_item->set_right_value(parse_begun_list());
  }

  return list_item;
}

void Parser::expect_token(const Token& tok, bool extract) {
  if (tok != tokenizer_->peek_token()) {
    std::stringstream ss;
    ss << "Expected " << tok << " got " << tokenizer_->peek_token();
    throw ParserError(ss.str());
  }

  if (extract) {
    tokenizer_->next_token();
  }
}

void Parser::skip_endlines() {
  while (tokenizer_->peek_token().type == TokenType::kEndLine) {
    tokenizer_->next_token();
  }
}

} // lispp
