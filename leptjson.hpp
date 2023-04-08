#ifndef LEPTJSON_H_
#define LEPTJSON_H_

#include <string>

namespace Lept {
enum class Type { Null, False, True, Number, String, Array, Object };

struct Value {
  Type type;
};

enum class Parse_error { ok, except_value, invalid_value, root_not_singular };

Parse_error parse(Value &v, const std::string &json);

Type get_type(const Value &v);
} // namespace Lept

#endif // LEPTJSON_H_
