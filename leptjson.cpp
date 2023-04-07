#include "leptjson.hpp"
#include <algorithm>
#include <locale>
#include <string_view>

namespace Lept_impl {
using Lept::Lept_type;
using Lept::lept_value;
using Lept::Parse_error;

struct lept_context {
  std::string json;
};

Parse_error parse(lept_value &v, const std::string &json);
void parse_whitespace(lept_context &c);
Parse_error parse_null(lept_context &c, lept_value &v);
Parse_error parse_value(lept_context &c, lept_value &v);
Parse_error parse_true(lept_context &c, lept_value &v);
Parse_error parse_false(lept_context &c, lept_value &v);

Lept::Parse_error parse_literal(lept_context &c, lept_value &v,
                                const std::string_view &literal,
                                Lept_type type);
}; // namespace Lept_impl

/*
** 解析json字符串，并将结果存储到lept_value结构体中
** v -- lept_value类型的引用，用于保存解析结果
** json is 需要解析的json字符串
** Precondition: v的类型为Null，json不能为空字符串
** Postcondition
** v的类型可能为Null、True、False或Number，或者为Array或Object类型（后续实现）
** 若解析成功，则返回Parse_error::ok；否则返回其他Parse_error类型的错误信息
** 该函数通过解析json字符串，将解析结果存储到传入的lept_value结构体中。
** 在解析过程中会调用其他解析函数，如parse_value和parse_whitespace等。
** 解析过程中可能会出现多种错误，如无效的值、不合法的语法等。
** 若解析成功，则返回Parse_error::ok；否则返回其他Parse_error类型的错误信息。
*/
Lept::Parse_error Lept_impl::parse(lept_value &v, const std::string &json) {
  lept_context c;
  c.json = std::move(json);
  v.type = Lept_type::Null;
  parse_whitespace(c);
  return parse_value(c, v);
}

// 从字符串 json 中删除前导空白字符，即制表符、空格、回车和换行符等
// 参数：lept_context &c，包含了要被解析的 JSON 字符串
// Precondition：lept_context 对象 c 已经初始化，并且其成员变量 json
// 中包含了要被解析的 JSON 字符串 Postcondition：字符串
// json中前导空白字符被删除，json 的值被更新为去掉前导空白字符的字符串
void Lept_impl::parse_whitespace(lept_context &c) {
  auto first_non_whitespace =
      std::find_if_not(c.json.begin(), c.json.end(), [](char ch) {
        return std::isspace(static_cast<unsigned char>(ch));
      });
  c.json.erase(c.json.begin(), first_non_whitespace);
}
// 这个函数 Lept_impl::parse_literal 用于解析 JSON 中的 null、true 和 false
// 三个字面值，并将解析结果存入 lept_value 类型的变量 v 中。
// 参数 c 是一个包含要解析的 JSON 字符串的 lept_context 类型的变量。
// 参数 v 是一个 lept_value 类型的变量，用于存储解析结果。
// 参数 literal 是一个 std::string_view 类型的变量，包含要解析的字面值。
// 参数 type 是一个 Lept_type 类型的变量，表示解析结果的类型，取值为
// Lept_type::Null、Lept_type::True 或 Lept_type::False。
// 函数首先检查要解析的字符串是否以指定的字面值开头，并检查开头之后是否还有其他字符。如果解析成功，则将解析结果的类型存入
// v.type 中，并返回 Lept::Parse_error::ok。如果解析失败，则返回相应的错误码。
// 此函数的前置条件是 c.json 包含一个以 literal 开头的字符串。函数执行完毕后，v
// 的类型将被设置为 type。
Lept::Parse_error Lept_impl::parse_literal(lept_context &c, lept_value &v,
                                           const std::string_view &literal,
                                           Lept_type type) {
  if (c.json.size() < literal.size() ||
      c.json.substr(0, literal.size()) != literal) {
    return Lept::Parse_error::invalid_value;
  } else if (c.json.size() > literal.size() and
             c.json[literal.size()] != '\0') {
    return Lept::Parse_error::root_not_singular;
  }
  v.type = type;
  return Lept::Parse_error::ok;
}

Lept::Parse_error Lept_impl::parse_null(lept_context &c, lept_value &v) {
  return parse_literal(c, v, "null", Lept_type::Null);
}

Lept::Parse_error Lept_impl::parse_true(lept_context &c, lept_value &v) {
  return parse_literal(c, v, "true", Lept_type::True);
}

Lept::Parse_error Lept_impl::parse_false(lept_context &c, lept_value &v) {
  return parse_literal(c, v, "false", Lept_type::False);
}

// 这是解析 JSON 值的函数，输入参数是一个 lept_context 对象和一个 lept_value
// 对象 该函数根据 JSON 字符串的第一个字符来决定要解析的 JSON
// 值类型，分别调用相应的解析函数 如果第一个字符不是
// 'n'、't'、'f'(只需要解析第一个字母) 或者字符串为空，则返回无效值错误
// 解析成功后，会将 lept_value 对象的 type 成员设置为相应的类型
// 函数返回一个枚举类型，表示解析过程中的错误类型
Lept::Parse_error Lept_impl::parse_value(lept_context &c, lept_value &v) {
  if (c.json[0] == 'n')
    return parse_null(c, v);
  else if (c.json[0] == 't')
    return parse_true(c, v);
  else if (c.json[0] == 'f')
    return parse_false(c, v);
  else if (c.json[0] == '\0')
    return Parse_error::except_value;

  return Parse_error::invalid_value;
}

Lept::Lept_type Lept::get_type(const lept_value &v) { return v.type; }

Lept::Parse_error Lept::parse(lept_value &v, const std::string &json) {
  return Lept_impl::parse(v, json);
}
