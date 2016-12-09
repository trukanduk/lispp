#include <sstream>
#include <gtest/gtest.h>

#include <lispp/tokenizer.h>
#include <lispp/string_tokenizer.h>

using namespace lispp;

class StringTokenizerTest : public ::testing::Test {
protected:
  StringTokenizer tokenizer;
};

TEST_F(StringTokenizerTest, TestEmpty) {
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer.peek_token());
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer.next_token());

  EXPECT_FALSE(tokenizer.has_more_tokens());
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer.peek_token());
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer.peek_token());
}

TEST_F(StringTokenizerTest, TestBasic) {
  tokenizer << "'";
  EXPECT_TRUE(tokenizer.has_more_tokens());

  EXPECT_EQ(Token(TokenType::kQuote), tokenizer.peek_token());
  EXPECT_EQ(Token(TokenType::kQuote), tokenizer.next_token());

  EXPECT_EQ(Token(TokenType::kEnd), tokenizer.peek_token());
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer.next_token());

  EXPECT_FALSE(tokenizer.has_more_tokens());
}

TEST_F(StringTokenizerTest, TestIncremental_NonFinished) {
  tokenizer << "'";
  EXPECT_TRUE(tokenizer.has_more_tokens());

  tokenizer << ",";
  EXPECT_TRUE(tokenizer.has_more_tokens());
  EXPECT_EQ(Token(TokenType::kQuote), tokenizer.next_token());
  EXPECT_EQ(Token(TokenType::kComma), tokenizer.next_token());
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer.next_token());

  EXPECT_FALSE(tokenizer.has_more_tokens());
}

TEST_F(StringTokenizerTest, TestIncremental_Finished) {
  tokenizer << "'";
  EXPECT_TRUE(tokenizer.has_more_tokens());
  EXPECT_EQ(Token(TokenType::kQuote), tokenizer.next_token());

  tokenizer << ",";
  EXPECT_TRUE(tokenizer.has_more_tokens());
  EXPECT_EQ(Token(TokenType::kComma), tokenizer.next_token());
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer.next_token());

  EXPECT_FALSE(tokenizer.has_more_tokens());
}

TEST_F(StringTokenizerTest, TestIncremental_Peeked) {
  tokenizer << "'";
  EXPECT_TRUE(tokenizer.has_more_tokens());
  EXPECT_EQ(Token(TokenType::kQuote), tokenizer.peek_token());

  tokenizer << ",";
  EXPECT_TRUE(tokenizer.has_more_tokens());
  EXPECT_EQ(Token(TokenType::kQuote), tokenizer.next_token());
  EXPECT_EQ(Token(TokenType::kComma), tokenizer.next_token());
  EXPECT_EQ(Token(TokenType::kEnd), tokenizer.next_token());

  EXPECT_FALSE(tokenizer.has_more_tokens());
}
