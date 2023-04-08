#include "leptjson.hpp"
#include <gtest/gtest.h>

TEST(leptjsonTest, null) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "null"));
  EXPECT_EQ(Lept::Type::Null, Lept::get_type(v));
}

TEST(leptjsonTest, true) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "true"));
  EXPECT_EQ(Lept::Type::True, Lept::get_type(v));
}

TEST(leptjsonTest, false) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "false"));
  EXPECT_EQ(Lept::Type::False, Lept::get_type(v));
}

TEST(leptjsonTest, invalidValue) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "haha"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "nul"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "trew"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "?"));
}

TEST(leptjsonTest, exceptValue) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::except_value, Lept::parse(v, ""));
  EXPECT_EQ(Lept::Parse_error::except_value, Lept::parse(v, " "));
}

TEST(leptjsonTest, rootNotSingular) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::root_not_singular, Lept::parse(v, "null x"));
  EXPECT_EQ(Lept::Parse_error::root_not_singular, Lept::parse(v, "true x"));
  EXPECT_EQ(Lept::Parse_error::root_not_singular, Lept::parse(v, "false x"));
}

TEST(leptjsonTest, zero) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "0"));
  EXPECT_EQ(Lept::Type::Number, Lept::get_type(v));
  EXPECT_EQ(0.0, Lept::get_number(v));
}

TEST(leptjsonTest, positiveInt) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "123"));
  EXPECT_EQ(Lept::Type::Number, Lept::get_type(v));
  EXPECT_EQ(123.0, Lept::get_number(v));
}

TEST(leptjsonTest, negativeInt) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "-456"));
  EXPECT_EQ(Lept::Type::Number, Lept::get_type(v));
  EXPECT_EQ(-456.0, Lept::get_number(v));
}

TEST(leptjsonTest, positiveFloat) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "3.14"));
  EXPECT_EQ(Lept::Type::Number, Lept::get_type(v));
  EXPECT_EQ(3.14, Lept::get_number(v));
}

TEST(leptjsonTest, negativeFloat) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "-2.71828"));
  EXPECT_EQ(Lept::Type::Number, Lept::get_type(v));
  EXPECT_EQ(-2.71828, Lept::get_number(v));
}

TEST(leptjsonTest, numberWithExponent) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "1.23e4"));
  EXPECT_EQ(Lept::Type::Number, Lept::get_type(v));
  EXPECT_EQ(1.23e4, Lept::get_number(v));
}

TEST(leptjsonTest, numberWithLeadingZeros) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::success, Lept::parse(v, "0000123"));
  EXPECT_EQ(Lept::Type::Number, Lept::get_type(v));
  EXPECT_EQ(123.0, Lept::get_number(v));
}

TEST(leptjsonTest, invalidNumberFormat) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "123abc"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "-123-"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "1.2.3"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "1E2.5"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "1E2.5k"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "-"));
}

TEST(leptjsonTest, bigNumber) {
  Lept::Value v;
  EXPECT_EQ(Lept::Parse_error::number_too_big, Lept::parse(v, "1e300000009"));
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
