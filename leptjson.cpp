#include "leptjson.hpp"
#include <algorithm>
#include <cassert>
#include <limits>
#include <locale>

namespace mp = boost::multiprecision;

namespace Lept_impl {
using Lept::Parse_error;
using Lept::Type;
using Lept::Value;

struct lept_context {
  std::string json;
};

Parse_error parse(Value &v, const std::string &json);
void parse_whitespace(lept_context &c);
Parse_error parse_null(lept_context &c, Value &v);
Parse_error parse_value(lept_context &c, Value &v);
Parse_error parse_true(lept_context &c, Value &v);
Parse_error parse_false(lept_context &c, Value &v);
Parse_error parse_number(lept_context &c, Value &v);

Lept::Parse_error parse_literal(lept_context &c, Value &v,
                                const std::string_view &literal, Type type);
}; // namespace Lept_impl

/*
** Parse a JSON string and store the result in a Value struct
** v -- Reference to a Value type used to store the parsing result
** json -- JSON string to be parsed
** Precondition: v is of type Null and json is a non-empty string
**
** Postcondition:
** v may be of type Null, True, False, Number, or an Array or Object type (to be
** implemented later)
**
** If parsing succeeds, returns Parse_error::success; otherwise, returns an
*error
** message of another Parse_error type
** This function parses the JSON string and stores the parsing result in the
** passed-in Value struct.
** Other parsing functions, such as parse_value and parse_whitespace, are called
** during parsing.
** Various errors may occur during parsing, such as invalid values, illegal
** syntax, etc.
** If parsing succeeds, returns Parse_error::success; otherwise, returns an
*error
** message of another Parse_error type.
*/
Lept::Parse_error Lept_impl::parse(Value &v, const std::string &json) {
  lept_context c;
  c.json = json;
  v.type = Type::Null;
  parse_whitespace(c);
  return parse_value(c, v);
}

// This function removes leading whitespace characters such as tabs, spaces,
// carriage returns, and newlines from the string 'json'.
// Parameter 'c' is a lept_context object that contains the JSON string to be
// parsed.
// Precondition: The lept_context object 'c' is initialized and its member
// variable 'json' contains the JSON string to be parsed.
// Postcondition: Leading whitespace characters in the string 'json' are
// removed and the value of 'json' is updated to the trimmed string.
void Lept_impl::parse_whitespace(lept_context &c) {
  auto first_non_whitespace =
      std::find_if_not(c.json.begin(), c.json.end(), [](char ch) {
        return std::isspace(static_cast<unsigned char>(ch));
      });
  c.json.erase(c.json.begin(), first_non_whitespace);
}

// This function Lept_impl::parse_literal is used to parse the three literal
// values "null", "true", and "false" in JSON, and store the parsing result in a
// Value variable v. Parameter c is a lept_context variable containing the JSON
// string to be parsed. Parameter v is a Value variable used to store the
// parsing result. Parameter literal is a std::string_view variable containing
// the literal value to be parsed. Parameter type is a Type variable indicating
// the expected parsing result type, which can be Type::Null, Type::True, or
// Type::False. The function first checks if the string to be parsed starts with
// the specified literal value and if there are no other characters after the
// literal. If the parsing succeeds, the function sets v.type to the expected
// type and returns Lept::Parse_error::success. If the parsing fails, the
// function returns the corresponding error code. The precondition for this
// function is that c.json contains a string starting with the specified literal
// value. After the function is executed, the type of v will be set to the
// expected type.
Lept::Parse_error Lept_impl::parse_literal(lept_context &c, Value &v,
                                           const std::string_view &literal,
                                           Type type) {
  assert(!c.json.empty());
  assert(!literal.empty());
  if (c.json.size() < literal.size() ||
      c.json.substr(0, literal.size()) != literal) {
    return Lept::Parse_error::invalid_value;
  } else if (c.json[literal.size()] != '\0') {
    return Lept::Parse_error::root_not_singular;
  }
  v.type = type;
  return Lept::Parse_error::success;
}

Lept::Parse_error Lept_impl::parse_null(lept_context &c, Value &v) {
  return parse_literal(c, v, "null", Type::Null);
}

Lept::Parse_error Lept_impl::parse_true(lept_context &c, Value &v) {
  return parse_literal(c, v, "true", Type::True);
}

Lept::Parse_error Lept_impl::parse_false(lept_context &c, Value &v) {
  return parse_literal(c, v, "false", Type::False);
}

// TODO: Support large number
// This function Lept_impl::parse_number is used to parse numbers in JSON,
// and store the parsing result in a Value variable v.
// Parameter c is a lept_context variable containing the JSON string to be
// parsed. If the parsing succeeds, the function sets v.type to Type::Number and
// v.n to the parsed number, and returns Lept::Parse_error::success.
// If the parsing fails, the function returns the corresponding error code.
// This function checks for negative sign, integer part, decimal part, and
// exponent part in the number. It also catches std::out_of_range exception when
// parsing very large numbers.
// NOTE: This implementation currently does not
// support parsing very large numbers. And may use from_chars instead
Lept::Parse_error Lept_impl::parse_number(lept_context &c, Value &v) try {
  const std::string &s = c.json;
  const size_t len = s.length();

  // Check for negative sign
  size_t i = 0;
  if (s[i] == '-') {
    if (++i == len || !isdigit(s[i]))
      return Parse_error::invalid_value;
  }

  // Check for integer part
  if (i < len && !isdigit(s[i]))
    return Parse_error::invalid_value;
  while (i < len && isdigit(s[i]))
    ++i;

  // Check for decimal part
  if (i < len && s[i] == '.') {
    ++i;
    if (i == len || !isdigit(s[i]))
      return Parse_error::invalid_value;
    while (i < len && isdigit(s[i]))
      ++i;
  }

  // Check for exponent part
  if (i < len && (s[i] == 'e' || s[i] == 'E')) {
    ++i;
    if (i < len && (s[i] == '+' || s[i] == '-'))
      ++i;
    if (i == len || !isdigit(s[i]))
      return Parse_error::invalid_value;
    while (i < len && isdigit(s[i]))
      ++i;
  }

  if (i != len)
    return Parse_error::invalid_value;

  v.type = Type::Number;
  v.data = std::stod(s);

  return Parse_error::success;
} catch (std::out_of_range &) {
  return Parse_error::number_too_big;
} catch (...) {
  return Parse_error::invalid_value;
}

// 这是解析 JSON 值的函数，输入参数是一个 lept_context 对象和一个 Value
// 对象 该函数根据 JSON 字符串的第一个字符来决定要解析的 JSON
// 值类型，分别调用相应的解析函数 如果第一个字符不是
// 'n'、't'、'f'(只需要解析第一个字母) 或者字符串为空，则返回无效值错误
// 解析成功后，会将 Value 对象的 type 成员设置为相应的类型
// 函数返回一个枚举类型，表示解析过程中的错误类型
Lept::Parse_error Lept_impl::parse_value(lept_context &c, Value &v) {
  switch (c.json[0]) {
  case 'n':
    return parse_null(c, v);
  case 't':
    return parse_true(c, v);
  case 'f':
    return parse_false(c, v);
  case '\0':
    return Parse_error::except_value;
  default:
    return parse_number(c, v);
  }
}

Lept::Type Lept::get_type(const Value &v) { return v.type; }

Lept::Parse_error Lept::parse(Value &v, const std::string &json) {
  return Lept_impl::parse(v, json);
}

mp::cpp_dec_float_100 Lept::get_number(const Value &v) {
  if (v.type == Type::Number) {
    return std::get<mp::cpp_dec_float_100>(v.data);
  } else
    return std::numeric_limits<double>::quiet_NaN();
}
