#include <sstream>
#include <gtest/gtest.h>

#include <lispp/tokenizer.h>
#include <lispp/istream_tokenizer.h>

using namespace lispp;

std::ostream& operator<<(std::ostream& out, const std::vector<Token>& tokens) {
  for (const auto& token : tokens) {
    out << token << ' ';
  }
  return out;
}

class IstreamTokenizerTest : public ::testing::Test {
protected:
  void exec(const std::string& s, bool check_tokens = true) {
    ss << s;
    tokenizer.reset(new IstreamTokenizer(ss));

    if (check_tokens) {
      EXPECT_TRUE(tokenizer->has_more_tokens());
    }
  }

  void read_tokens(int num_tokens = -1) {
    tokens.clear();
    do {
      tokens.push_back(tokenizer->next_token());
    } while (tokens.back().type != TokenType::kEnd &&
             num_tokens != -1 && static_cast<int>(tokens.size()) < num_tokens);
  }

  template<typename... Tokens>
  void expect_tokens(const Tokens&... rest_tokens) {
    ++num_series;
    std::vector<Token> expected_tokens{rest_tokens...};
    EXPECT_NO_THROW(read_tokens(expected_tokens.size())) << "Got " << tokens;

    EXPECT_EQ(expected_tokens.size(), tokens.size())
        << "num_series = " << num_series << "\n"
        << "Expected " << expected_tokens << "\n"
        << "Got      " << tokens;

    for (std::size_t i = 0; i < tokens.size(); ++i) {
      EXPECT_EQ(expected_tokens[i], tokens[i])
        << "num_series = " << num_series << "\n"
        << "Expected " << expected_tokens << "\n"
        << "Got      " << tokens << "\n"
        << "Differs at " << i;
    }
  }

  void expect_fail(int num_tokens = 1) {
    EXPECT_THROW(read_tokens(num_tokens), TokenizerError) << "Got " << tokens;
  }

  std::stringstream ss;
  std::unique_ptr<IstreamTokenizer> tokenizer;
  std::vector<Token> tokens;
  int num_series = 0;
};

TEST_F(IstreamTokenizerTest, EmptySequence) {
  exec("", false);
  EXPECT_FALSE(tokenizer->has_more_tokens());

  expect_tokens(
      Token(TokenType::kEnd)
  );
  EXPECT_FALSE(tokenizer->has_more_tokens());
}

TEST_F(IstreamTokenizerTest, EmptySequence_AfterEnd) {
  exec("", false);
  EXPECT_FALSE(tokenizer->has_more_tokens());

  expect_tokens(
      Token(TokenType::kEnd)
  );
  EXPECT_FALSE(tokenizer->has_more_tokens());

  expect_tokens(
      Token(TokenType::kEnd)
  );
  EXPECT_FALSE(tokenizer->has_more_tokens());
}

TEST_F(IstreamTokenizerTest, Miltiline) {
  exec(
      "1 2\n"
      "3 4"
  );
  expect_tokens(
      Token(TokenType::kNumber, 1),
      Token(TokenType::kNumber, 2),
      Token(TokenType::kEndLine),
      Token(TokenType::kNumber, 3),
      Token(TokenType::kNumber, 4),
      Token(TokenType::kEnd)
  );
  EXPECT_FALSE(tokenizer->has_more_tokens());
}

TEST_F(IstreamTokenizerTest, MultiLine_EmptyLine) {
  exec(
      "1\n"
      "\n"
      "2"
  );
  expect_tokens(
      Token(TokenType::kNumber, 1),
      Token(TokenType::kEndLine)
  );
  EXPECT_TRUE(tokenizer->has_more_tokens());

  expect_tokens(
      Token(TokenType::kEndLine)
  );
  EXPECT_TRUE(tokenizer->has_more_tokens());

  expect_tokens(
      Token(TokenType::kNumber, 2),
      Token(TokenType::kEnd)
  );
  EXPECT_FALSE(tokenizer->has_more_tokens());
}

TEST_F(IstreamTokenizerTest, CurrentToken_Initial) {
  exec("1");
  EXPECT_EQ(Token(), tokenizer->current_token());
  EXPECT_EQ(Token(), tokenizer->current_token());
}

TEST_F(IstreamTokenizerTest, CurrentToken) {
  exec("1");
  EXPECT_EQ(Token(TokenType::kNumber, 1), tokenizer->next_token());

  EXPECT_EQ(Token(TokenType::kNumber, 1), tokenizer->current_token());
  EXPECT_EQ(Token(TokenType::kNumber, 1), tokenizer->current_token());
}

TEST_F(IstreamTokenizerTest, PeekToken_Initial) {
  exec("1");

  EXPECT_EQ(Token(TokenType::kNumber, 1), tokenizer->peek_token());
  EXPECT_EQ(Token(TokenType::kNumber, 1), tokenizer->peek_token());

  EXPECT_EQ(Token(), tokenizer->current_token());
}

TEST_F(IstreamTokenizerTest, PeekToken) {
  exec("1 2");

  EXPECT_EQ(Token(TokenType::kNumber, 1), tokenizer->next_token());

  EXPECT_EQ(Token(TokenType::kNumber, 2), tokenizer->peek_token());
  EXPECT_EQ(Token(TokenType::kNumber, 2), tokenizer->peek_token());
}

TEST_F(IstreamTokenizerTest, IncrementalAdditions) {
  exec("1");
  EXPECT_EQ(Token(TokenType::kNumber, 1), tokenizer->next_token());
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer->peek_token());
  EXPECT_FALSE(tokenizer->has_more_tokens());

  ss.clear();
  ss << " 2";
  EXPECT_TRUE(tokenizer->has_more_tokens());
  EXPECT_EQ(Token(TokenType::kNumber, 2), tokenizer->peek_token());
  EXPECT_EQ(Token(TokenType::kNumber, 2), tokenizer->next_token());
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer->peek_token());
  EXPECT_FALSE(tokenizer->has_more_tokens());
}


class IstreamTokenizerTest_SingleTokens : public IstreamTokenizerTest {};

TEST_F(IstreamTokenizerTest_SingleTokens, Characters) {
  exec("\"foo bar 213 ()&\"");
  expect_tokens(
      Token(TokenType::kCharacters, "foo bar 213 ()&"),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_SingleTokens, DISABLED_Characters_Escaped) {
  exec("\"\\\"\"");
  expect_tokens(
      Token(TokenType::kCharacters, "\""),
      Token(TokenType::kEnd)
  );
}


TEST_F(IstreamTokenizerTest_SingleTokens, Comma) {
  exec(",");
  expect_tokens(
      Token(TokenType::kComma),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_SingleTokens, BackTick) {
  exec("`");
  expect_tokens(
      Token(TokenType::kBackTick),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_SingleTokens, Dot) {
  exec(".");
  expect_tokens(
      Token(TokenType::kDot),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_SingleTokens, Quote) {
  exec("'");
  expect_tokens(
      Token(TokenType::kQuote),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_SingleTokens, OpenBracket) {
  exec("(");
  expect_tokens(
      Token(TokenType::kOpenBracket),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_SingleTokens, CloseBracket) {
  exec(")");
  expect_tokens(
      Token(TokenType::kCloseBracket),
      Token(TokenType::kEnd)
  );
}

class IstreamTokenizerTest_Numbers : public IstreamTokenizerTest {};

TEST_F(IstreamTokenizerTest_Numbers, IntNumber_OneDigit) {
  exec("1");
  expect_tokens(
      Token(TokenType::kNumber, 1),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, IntNumber_MultipleDigits) {
  exec("1123");
  expect_tokens(
      Token(TokenType::kNumber, 1123),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, IntNumber_Plus) {
  exec("+1123");
  expect_tokens(
      Token(TokenType::kNumber, 1123),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, IntNumber_Minus) {
  exec("-1123");
  expect_tokens(
      Token(TokenType::kNumber, -1123),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, IntNumber_DoubleSigns) {
  exec("+11-23");
  expect_fail();
}

TEST_F(IstreamTokenizerTest_Numbers, IntNumber_InvalidSign) {
  exec("11-23");
  expect_fail();
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_OneDigit) {
  exec("0.1");
  expect_tokens(
      Token(TokenType::kNumber, 0.1),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_MultipleDigitsInt) {
  exec("123.4");
  expect_tokens(
      Token(TokenType::kNumber, 123.4),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_MultipleDigitsFrac) {
  exec("0.123");
  expect_tokens(
      Token(TokenType::kNumber, 0.123),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_MultipleDigits) {
  exec("456.123");
  expect_tokens(
      Token(TokenType::kNumber, 456.123),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_FracOnly) {
  exec(".123");
  expect_tokens(
      Token(TokenType::kNumber, 0.123),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_IntOnly) {
  exec("123.");
  expect_tokens(
      Token(TokenType::kNumber, 123.0),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_Plus) {
  exec("+1.2");
  expect_tokens(
      Token(TokenType::kNumber, 1.2),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_Minus) {
  exec("-1.2");
  expect_tokens(
      Token(TokenType::kNumber, -1.2),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_PlusFracOnly) {
  exec("+.2");
  expect_tokens(
      Token(TokenType::kNumber, 0.2),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_MinusFracOnly) {
  exec("-.2");
  expect_tokens(
      Token(TokenType::kNumber, -0.2),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_PlusIntOnly) {
  exec("+2.");
  expect_tokens(
      Token(TokenType::kNumber, 2.0),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_MinusIntOnly) {
  exec("-2.");
  expect_tokens(
      Token(TokenType::kNumber, -2.0),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Numbers, RealNumber_MultipleDots) {
  exec("2.2.2");
  expect_fail();
}

class IstreamTokenizerTest_Symbols : public IstreamTokenizerTest_Numbers {};

TEST_F(IstreamTokenizerTest_Symbols, Symbol_Simple) {
  exec("foo");
  expect_tokens(
      Token(TokenType::kSymbol, "foo"),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Symbols, Symbol_Multiword) {
  exec("foo-bar");
  expect_tokens(
      Token(TokenType::kSymbol, "foo-bar"),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Symbols, Symbol_Plus) {
  exec("+");
  expect_tokens(
      Token(TokenType::kSymbol, "+"),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Symbols, Symbol_Minus) {
  exec("-");
  expect_tokens(
      Token(TokenType::kSymbol, "-"),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Symbols, Symbol_StartPlus) {
  exec("+foo");
  expect_fail();
}

TEST_F(IstreamTokenizerTest_Symbols, Symbol_StartMinus) {
  exec("-foo");
  expect_fail();
}

TEST_F(IstreamTokenizerTest_Symbols, Symbol_TrueToken) {
  exec("#t");
  expect_tokens(
      Token(TokenType::kSymbol, "#t"),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Symbols, Symbol_RestToken) {
  exec("&rest");
  expect_tokens(
      Token(TokenType::kSymbol, "&rest"),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Symbols, Symbol_WithDigits) {
  exec("min2");
  expect_tokens(
      Token(TokenType::kSymbol, "min2"),
      Token(TokenType::kEnd)
  );
}

class IstreamTokenizerTest_Composite : public IstreamTokenizerTest {};

TEST_F(IstreamTokenizerTest_Composite, ListNode) {
  exec("(a . b)");
  expect_tokens(
      Token(TokenType::kOpenBracket),
      Token(TokenType::kSymbol, "a"),
      Token(TokenType::kDot),
      Token(TokenType::kSymbol, "b"),
      Token(TokenType::kCloseBracket),
      Token(TokenType::kEnd)
  );
}

TEST_F(IstreamTokenizerTest_Composite, SimpleList) {
  exec("(1 2 \"foo bar\" (3 . 4))");
  expect_tokens(
      Token(TokenType::kOpenBracket),
      Token(TokenType::kNumber, 1),
      Token(TokenType::kNumber, 2),
      Token(TokenType::kCharacters, "foo bar"),
      Token(TokenType::kOpenBracket),
      Token(TokenType::kNumber, 3),
      Token(TokenType::kDot),
      Token(TokenType::kNumber, 4),
      Token(TokenType::kCloseBracket),
      Token(TokenType::kCloseBracket),
      Token(TokenType::kEnd)
  );
}
