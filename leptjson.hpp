#ifndef LEPTJSON_H_
#define LEPTJSON_H_

#include <string>

namespace Lept {
enum class Lept_type : int { Null, False, True, Number, String, Array, Object };

struct lept_value {
  Lept_type type;
};

enum class Parse_error { ok, except_value, invalid_value, root_not_singular };

Parse_error parse(lept_value &v, const std::string &json);

Lept_type get_type(const lept_value &v);
} // namespace Lept

#endif // LEPTJSON_H_
