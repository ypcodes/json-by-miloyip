#ifndef LEPTJSON_H_
#define LEPTJSON_H_

#include <string>
#include <variant>

namespace Lept {
enum class Type { Null, False, True, Number, String, Array, Object };

// I use std::variant instead of union
struct Value {
  std::variant<double, std::string> data;
  Type type{Type::Null};
};

enum class Parse_error {
  success,
  except_value,
  invalid_value,
  root_not_singular,
  number_too_big,
  miss_quotation_mark,
  invalid_string_escape,
  invalid_string_char
};

Parse_error parse(Value &v, const std::string &json);

Type get_type(const Value &v);
double get_number(const Value &v);
std::string get_string(const Value &v);
} // namespace Lept

#endif // LEPTJSON_H_
