#include "leptjson.hpp"
#include <algorithm>
#include <cassert>
#include <charconv>
#include <limits>
#include <locale>
#include <stdexcept>
#include <string>
#include <variant>

namespace Lept_impl {
using Lept::Parse_error;
using Lept::Type;
using Lept::Value;

struct lept_context {
  std::string json;
};

Parse_error parse(Value &v, const std::string &json);
void parse_whitespace(lept_context &c);
Parse_error parse_null(const lept_context &c, Value &v);
Parse_error parse_value(const lept_context &c, Value &v);
Parse_error parse_true(const lept_context &c, Value &v);
Parse_error parse_false(const lept_context &c, Value &v);
Parse_error parse_number(const lept_context &c, Value &v);
Parse_error parse_string(const lept_context &c, Value &v);

Parse_error parse_literal(const lept_context &c, Value &v,
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

Lept::Parse_error Lept_impl::parse(Value &val, const std::string &json) {
  // it is suitable to use =std::move= in the given code snippet since =json= is
  // no longer needed after it is moved into =c=. It can help reduce unnecessary
  // copying and improve performance.
  lept_context context{std::move(json)};
  // c.json = json;
  val.type = Type::Null;
  parse_whitespace(context);
  return parse_value(context, val);
}

// This function removes leading whitespace characters such as tabs, spaces,
// carriage returns, and newlines from the string 'json'.
// Parameter 'c' is a const lept_context object that contains the JSON string to
// be parsed. Precondition: The const lept_context object 'c' is initialized and
// its member variable 'json' contains the JSON string to be parsed.
// Postcondition: Leading whitespace characters in the string 'json' are
// removed and the value of 'json' is updated to the trimmed string.
// why use static_cast<unsigned char>, and how to avoid

// Using =static_cast<unsigned char>= is used to ensure that the input character
// is cast to an unsigned value in the range [0, 255]. This is important because
// the =std::isspace= function requires a valid value in this range as input,
// otherwise the behavior is undefined.

// To avoid using =static_cast<unsigned char>=, one can instead use a character
// type that is already unsigned, such as =unsigned char=, =uint8_t=, or
// =std::byte=.
void Lept_impl::parse_whitespace(lept_context &c) {
  auto first_non_whitespace =
      std::find_if_not(c.json.begin(), c.json.end(), [](char ch) {
        return std::isspace(static_cast<unsigned char>(ch));
      });
  c.json.erase(c.json.begin(), first_non_whitespace);
}

// This function Lept_impl::parse_literal is used to parse the three literal
// values "null", "true", and "false" in JSON, and store the parsing result in a
// Value variable v. Parameter c is a const lept_context variable containing the
// JSON string to be parsed. Parameter v is a Value variable used to store the
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
Lept::Parse_error Lept_impl::parse_literal(
    const lept_context &c, Value &v,
    const std::string_view &literal, /*literal for right type*/
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

inline Lept::Parse_error Lept_impl::parse_null(const lept_context &c,
                                               Value &v) {
  return parse_literal(c, v, "null", Type::Null);
}

inline Lept::Parse_error Lept_impl::parse_true(const lept_context &c,
                                               Value &v) {
  return parse_literal(c, v, "true", Type::True);
}

inline Lept::Parse_error Lept_impl::parse_false(const lept_context &c,
                                                Value &v) {
  return parse_literal(c, v, "false", Type::False);
}

namespace {

static void parse_int_part_helper(const std::string &s, size_t &i) {
  while (isdigit(s[i]))
    ++i;
}

static void parse_decimal_part_helper(const std::string &s, size_t &i) {
  if (s[i] == '.') {
    ++i;
    if (!isdigit(s[i]))
      throw Lept::Parse_error::invalid_value;
    while (isdigit(s[i]))
      ++i;
  }
}

static void parse_exp_part_helper(const std::string &s, size_t &i) {
  if (i < s.length() && (s[i] == 'e' or s[i] == 'E')) {
    ++i;
    if (s[i] == '+' or s[i] == '-')
      ++i;
    if (!isdigit(s[i]))
      throw Lept::Parse_error::invalid_value;
    while (isdigit(s[i]))
      ++i;
  }
}
} // namespace

// TODO: Support large number
// TODO: refactor code until loc <= 10

// This function is a member function of the Lept_impl class. It takes a
// reference to a lept_context object and a reference to a Value object. It
// tries to parse a JSON number from the json string in the lept_context object
// and store the result in the Value object. It returns a Lept::Parse_error
// value indicating whether the parsing was successful and, if not, what went
// wrong. Parameters const lept_context &c: a reference to a lept_context object
// Value &v: a reference to a Value object Return value Lept::Parse_error: an
// enumeration value indicating the result of the parsing operation. Possible
// values are: Lept::Parse_error::success: the parsing was successful.
// Lept::Parse_error::invalid_value: the JSON number was invalid.
// Lept::Parse_error::number_too_big: the JSON number was too big to fit in a
// double.
Lept::Parse_error Lept_impl::parse_number(const lept_context &c, Value &v) {
  const std::string &s = c.json;
  size_t i = 0;

  if (s[i] == '-')
    ++i;
  if (!isdigit(s[i]))
    return Parse_error::invalid_value;

  // a number should looks like -123.2673E7182
  parse_int_part_helper(s, i);
  parse_decimal_part_helper(s, i);
  parse_exp_part_helper(s, i);

  if (i != s.length())
    return Parse_error::invalid_value;

  auto [_, ec]{std::from_chars(s.data(), s.data() + s.length(),
                               std::get<double>(v.data))};

  if (ec == std::errc::result_out_of_range)
    return Parse_error::number_too_big;
  else if (ec == std::errc::invalid_argument)
    return Parse_error::invalid_value;

  v.type = Type::Number;

  return Parse_error::success;
}

Lept::Parse_error Lept_impl::parse_string(const lept_context &c, Value &v) {
  // since c enter here only if c.json[0] == ", there is no need to put assert
  // assert(c.json[0] == '"');
  std::string str;
  size_t pos{1};
  while (pos < c.json.size()) {
    char ch{c.json[pos++]};
    if (ch == '"') {
      v.data = std::move(str);
      v.type = Type::String;
      return Parse_error::success;
    } else if (ch == '\\') {
      if (pos == c.json.size()) {
        return Parse_error::miss_quotation_mark;
      }
      // ", \, b, f, n, r,t, u...
      switch (c.json[pos++]) {
      case '"':
        str.push_back('"');
        break;
      case '\\':
        str.push_back('\\');
        break;
      case '/':
        str.push_back('/');
        break;
      case 'b':
        str.push_back('\b');
        break;
      case 'f':
        str.push_back('\f');
        break;
      case 'n':
        str.push_back('\n');
        break;
      case 'r':
        str.push_back('\r');
        break;
      case 't':
        str.push_back('\t');
        break;
      case 'u': {
        // TODO: add unicode parser
      } break;
      default:
        return Parse_error::invalid_string_escape;
      }
    } else if (static_cast<unsigned char>(ch) < 0x20)
      return Parse_error::invalid_string_char;
    else {
      str.push_back(ch);
    }
  }
  return Parse_error::miss_quotation_mark;
}

// 这是解析 JSON 值的函数，输入参数是一个 const lept_context 对象和一个 Value
// 对象 该函数根据 JSON 字符串的第一个字符来决定要解析的 JSON
// 值类型，分别调用相应的解析函数 解析成功后，会将 Value 对象的 type
// 成员设置为相应的类型 函数返回一个枚举类型(defined in
// leptjson.hpp)，表示解析过程中的错误类型
Lept::Parse_error Lept_impl::parse_value(const lept_context &c, Value &v) {
  // 如果检查整个c.json，root_not_signluar会和invalid_type等错误分不开
  switch (c.json[0]) {
  case 'n':
    return parse_null(c, v);
  case 't':
    return parse_true(c, v);
  case 'f':
    return parse_false(c, v);
  case '"':
    return parse_string(c, v);
  case '\0':
    return Parse_error::except_value;
  default:
    return parse_number(c, v);
  }
}

namespace {
std::string type_to_string(Lept::Type t) {
  switch (t) {
  case Lept::Type::Null:
    return "null";
  case Lept::Type::False:
    return "false";
  case Lept::Type::True:
    return "true";
  case Lept::Type::Number:
    return "number";
  case Lept::Type::String:
    return "string";
  case Lept::Type::Array:
    return "array";
  case Lept::Type::Object:
    return "object";
  default:
    return "unknown";
  }
}

// need add object and array
// get value helper
template <typename T> T get_val_helper(const Lept::Value &v) {
  if constexpr (std::is_same_v<T, double>) {
    if (v.type != Lept::Type::Number) {
      throw std::runtime_error(
          "Value is not a number (type =" + type_to_string(v.type) + ")");
    }
    return std::get<T>(v.data);
  }

  if constexpr (std::is_same_v<T, bool>) {
    if (v.type != Lept::Type::True and v.type != Lept::Type::False) {
      throw std::runtime_error(
          "Value is not a boolean (type =" + type_to_string(v.type) + ")");
    }
    return (v.type == Lept::Type::True) ? true : false;
  }

  if constexpr (std::is_same_v<T, std::string>) {
    if (v.type != Lept::Type::String) {
      throw std::runtime_error(
          "Value is not a string (type =" + type_to_string(v.type) + ")");
    }
    return std::get<T>(v.data);
  }

  throw std::runtime_error("Unsuported type");
}

} // namespace

Lept::Type Lept::get_type(const Value &v) { return v.type; }

Lept::Parse_error Lept::parse(Value &v, const std::string &json) {
  return Lept_impl::parse(v, json);
}

double Lept::get_number(const Value &v) { return get_val_helper<double>(v); }

std::string Lept::get_string(const Value &v) {
  return get_val_helper<std::string>(v);
}
