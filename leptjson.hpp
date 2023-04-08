#ifndef LEPTJSON_H_
#define LEPTJSON_H_

#include <string>

namespace Lept {
enum class Type { Null, False, True, Number, String, Array, Object };

struct Value {
  double n;
  Type type;
};

enum class Parse_error {
  ok,
  except_value,
  invalid_value,
  root_not_singular,
  number_too_big
};

Parse_error parse(Value &v, const std::string &json);

Type get_type(const Value &v);
double get_number(const Value &v);
} // namespace Lept

#endif // LEPTJSON_H_
