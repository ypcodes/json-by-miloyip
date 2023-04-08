#ifndef LEPTJSON_H_
#define LEPTJSON_H_

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <string>
#include <variant>

namespace Lept {
namespace mp = boost::multiprecision;

enum class Type { Null, False, True, Number, String, Array, Object };

// I use std::variant instead of union
struct Value {
  std::variant<mp::cpp_dec_float_100, std::string> data;
  Type type;
};

enum class Parse_error {
  success,
  except_value,
  invalid_value,
  root_not_singular,
  number_too_big
};

Parse_error parse(Value &v, const std::string &json);

Type get_type(const Value &v);
mp::cpp_dec_float_100 get_number(const Value &v);
} // namespace Lept

#endif // LEPTJSON_H_
