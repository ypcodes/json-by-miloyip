#include "leptjson.hpp"
#include <gtest/gtest.h>

TEST(leptjsonTest, null) {
  Lept::lept_value v;
  EXPECT_EQ(Lept::Parse_error::ok, Lept::parse(v, "null"));
  EXPECT_EQ(Lept::Lept_type::Null, Lept::get_type(v));
}

TEST(leptjsonTest, true) {
  Lept::lept_value v;
  EXPECT_EQ(Lept::Parse_error::ok, Lept::parse(v, "true"));
  EXPECT_EQ(Lept::Lept_type::True, Lept::get_type(v));
}

TEST(leptjsonTest, false) {
  Lept::lept_value v;
  EXPECT_EQ(Lept::Parse_error::ok, Lept::parse(v, "false"));
  EXPECT_EQ(Lept::Lept_type::False, Lept::get_type(v));
}

TEST(leptjsonTest, invalidValue) {
  Lept::lept_value v;
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "haha"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "nul"));
  EXPECT_EQ(Lept::Parse_error::invalid_value, Lept::parse(v, "trew"));
}

TEST(leptjsonTest, exceptValue) {
  Lept::lept_value v;
  EXPECT_EQ(Lept::Parse_error::except_value, Lept::parse(v, ""));
}

TEST(leptjsonTest, rootNotSingular) {
  Lept::lept_value v;
  EXPECT_EQ(Lept::Parse_error::root_not_singular, Lept::parse(v, "null x"));
  EXPECT_EQ(Lept::Parse_error::root_not_singular, Lept::parse(v, "true x"));
  EXPECT_EQ(Lept::Parse_error::root_not_singular, Lept::parse(v, "false x"));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
