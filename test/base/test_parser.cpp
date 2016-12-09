#include <sstream>
#include <gtest/gtest.h>

#include <lispp/string_tokenizer.h>
#include <lispp/tokenizer.h>
#include <lispp/parser.h>

#include <lispp/object_ptr.h>
#include <lispp/objects_all.h>

using namespace lispp;

class TokensTokenizer : public ITokenizer {
public:
  TokensTokenizer(std::initializer_list<Token> tokens) : tokens_(tokens) {}
  ~TokensTokenizer() override {}

  Token current_token() override {
    if (index_ < 0) {
      return Token();
    } else if (index_ >= static_cast<int>(tokens_.size())) {
      return Token(TokenType::kEnd);
    }

    return tokens_[index_];
  }

  Token peek_token() override {
    if (index_ + 1 >= static_cast<int>(tokens_.size())) {
      return Token(TokenType::kEnd);
    }

    return tokens_[index_ + 1];
  }

  Token next_token() override {
    if (index_ < static_cast<int>(tokens_.size())) {
      ++index_;
    }

    return current_token();
  }

  bool has_more_tokens() override {
    return (index_ != static_cast<int>(tokens_.size()));
  }

  int get_current_line() override { return 0; }
  void clear() override {}

private:
  std::vector<Token> tokens_;
  int index_ = -1;
};

class ParserTest : public ::testing::Test {
protected:
  void exec_string(const std::string& line) {
    tokenizer.reset(new StringTokenizer(line));
    parser.reset(new Parser(tokenizer.get()));
  }

  template<typename... Tokens>
  void exec_tokens(const Tokens&... tokens) {
    tokenizer.reset(new TokensTokenizer{tokens...});
    parser.reset(new Parser(tokenizer.get()));
  }

  std::unique_ptr<ITokenizer> tokenizer;
  std::unique_ptr<Parser> parser;
};

TEST_F(ParserTest, Empty) {
  exec_tokens();

  ObjectPtr<Object> obj(parser->parse_object());
  EXPECT_FALSE(obj.valid());
}

TEST_F(ParserTest, Number) {
  exec_tokens(
    Token(TokenType::kNumber, 123)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(NumberObject(123), *obj);
}

TEST_F(ParserTest, Characters) {
  exec_tokens(
    Token(TokenType::kCharacters, "foo")
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(CharactersObject("foo"), *obj);
}

TEST_F(ParserTest, Symbol) {
  exec_tokens(
    Token(TokenType::kSymbol, "foo")
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(SymbolObject("foo"), *obj);
}

TEST_F(ParserTest, EmptyList) {
  // ()
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_FALSE(obj.valid());
}

TEST_F(ParserTest, SingleListPair) {
  // (1 . 2)
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 1),
    Token(TokenType::kDot),
    Token(TokenType::kNumber, 2),
    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(ConsObject(new NumberObject(1), new NumberObject(2)), *obj);
}

TEST_F(ParserTest, List) {
  // (1 2)
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 1),
    Token(TokenType::kNumber, 2),
    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(ConsObject(new NumberObject(1),
                           new ConsObject(new NumberObject(2))),
            *obj);
}

TEST_F(ParserTest, ListWithDot) {
  // (1 2 . 3)
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 1),
    Token(TokenType::kNumber, 2),
    Token(TokenType::kDot),
    Token(TokenType::kNumber, 3),
    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(ConsObject(new NumberObject(1),
                           new ConsObject(new NumberObject(2),
                                              new NumberObject(3))),
            *obj);
}

TEST_F(ParserTest, NestedDotList) {
  // (1 . (2 . 3))
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 1),

    Token(TokenType::kDot),

    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 2),
    Token(TokenType::kDot),
    Token(TokenType::kNumber, 3),
    Token(TokenType::kCloseBracket),

    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(
      ConsObject(
          new NumberObject(1),
          new ConsObject(new NumberObject(2), new NumberObject(3))
      ),
      *obj
  );
}

TEST_F(ParserTest, NestedList_Simple) {
  // ((2 . 3) . 1)
  exec_tokens(
    Token(TokenType::kOpenBracket),

    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 2),
    Token(TokenType::kDot),
    Token(TokenType::kNumber, 3),
    Token(TokenType::kCloseBracket),

    Token(TokenType::kDot),

    Token(TokenType::kNumber, 1),
    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(
      ConsObject(
          new ConsObject(
              new NumberObject(2),
              new NumberObject(3)),
          new NumberObject(1)
      ),
      *obj
  );
}

TEST_F(ParserTest, NestedList_Complex) {
  // (1 (2 . 3) . (4 5))
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 1),

    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 2),
    Token(TokenType::kDot),
    Token(TokenType::kNumber, 3),
    Token(TokenType::kCloseBracket),

    Token(TokenType::kDot),

    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 4),
    Token(TokenType::kNumber, 5),
    Token(TokenType::kCloseBracket),

    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(
      ConsObject(
          new NumberObject(1),
          new ConsObject(
              new ConsObject(new NumberObject(2),
                                 new NumberObject(3)),
              new ConsObject(
                  new NumberObject(4),
                  new ConsObject(new NumberObject(5))
              )
          )
      ),
      *obj
  );
}

TEST_F(ParserTest, QuotedList) {
  // '(1 2)
  exec_tokens(
    Token(TokenType::kQuote),
    Token(TokenType::kOpenBracket),

    Token(TokenType::kNumber, 1),
    Token(TokenType::kNumber, 2),

    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(QuoteObject(new ConsObject(new NumberObject(1),
                                       new ConsObject(new NumberObject(2)))),
            *obj);
}

TEST_F(ParserTest, QuotedList_Empty) {
  // '()
  exec_tokens(
    Token(TokenType::kQuote),
    Token(TokenType::kOpenBracket),

    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  ASSERT_NE(nullptr, obj->as_quote());
  ASSERT_FALSE(obj->as_quote()->get_value().valid());
}

TEST_F(ParserTest, DoubleQuotedList) {
  // ''(1 2)
  exec_tokens(
    Token(TokenType::kQuote),
    Token(TokenType::kQuote),
    Token(TokenType::kOpenBracket),

    Token(TokenType::kNumber, 1),
    Token(TokenType::kNumber, 2),

    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(
      QuoteObject(new QuoteObject(
          new ConsObject(new NumberObject(1),
                         new ConsObject(new NumberObject(2))))
      ),
      *obj
  );
}

TEST_F(ParserTest, BackTickedList) {
  // `(1 ,2)
  exec_tokens(
    Token(TokenType::kBackTick),
    Token(TokenType::kOpenBracket),

    Token(TokenType::kNumber, 1),

    Token(TokenType::kComma),
    Token(TokenType::kNumber, 2),

    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  ASSERT_TRUE(obj.valid());
  EXPECT_EQ(
      BackTickObject(
          new ConsObject(new NumberObject(1),
                         new ConsObject(new CommaObject(new NumberObject(2))))
      ),
      *obj);
}

TEST_F(ParserTest, MultilineList_BetweenNumbers) {
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 123),

    Token(TokenType::kEndLine),

    Token(TokenType::kNumber, 321),
    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  EXPECT_EQ(
      ConsObject(
          new NumberObject(123),
          new ConsObject(new NumberObject(321))
      ),
      *obj
  );
}

TEST_F(ParserTest, MultilineList_BeforeNumbers) {
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kEndLine),
    Token(TokenType::kNumber, 123),
    Token(TokenType::kNumber, 321),
    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  EXPECT_EQ(
      ConsObject(
          new NumberObject(123),
          new ConsObject(new NumberObject(321))
      ),
      *obj
  );
}

TEST_F(ParserTest, MultilineList_BeforeClose) {
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kNumber, 123),
    Token(TokenType::kNumber, 321),
    Token(TokenType::kEndLine),
    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  EXPECT_EQ(
      ConsObject(
          new NumberObject(123),
          new ConsObject(new NumberObject(321))
      ),
      *obj
  );
}

TEST_F(ParserTest, MultilineList_Empty) {
  exec_tokens(
    Token(TokenType::kOpenBracket),
    Token(TokenType::kEndLine),
    Token(TokenType::kCloseBracket)
  );

  ObjectPtr<Object> obj(parser->parse_object());
  EXPECT_FALSE(obj.valid());
}
