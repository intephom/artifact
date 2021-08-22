#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace afct {

class Expr;

}

namespace std {

template<>
struct hash<afct::Expr>
{
  size_t operator()(afct::Expr const& expr) const;
};

} // namespace std

namespace afct {

enum class Type
{
  Null,
  Bool,
  Double,
  Int,
  String,
  Name,
  Lambda,
  Builtin,
  List,
  Table
};

struct String
{
  std::string s;
};

struct Name
{
  std::string n;
};

class Expr;
struct Lambda;
struct Builtin;
using List = std::vector<Expr>;
using Table = std::unordered_map<Expr, Expr>;

class Expr
{
public:
  Expr();
  explicit Expr(Type type);
  explicit Expr(bool b);
  explicit Expr(const char*) = delete; // stop implicit conversion to bool
  explicit Expr(double d);
  explicit Expr(int32_t i);
  explicit Expr(int64_t i);
  explicit Expr(String s);
  explicit Expr(Name n);
  explicit Expr(Lambda l);
  explicit Expr(Builtin b);
  explicit Expr(List l);
  explicit Expr(Table t);
  bool is_null() const;
  bool is_bool() const;
  bool is_double() const;
  bool is_int() const;
  bool is_numeric() const;
  bool is_string() const;
  bool is_name() const;
  bool is_lambda() const;
  bool is_builtin() const;
  bool is_function() const;
  bool is_list() const;
  bool is_table() const;
  Type get_type() const;
  bool get_bool() const;
  double get_double() const;
  int64_t get_int() const;
  double get_numeric() const;
  std::string const& get_string() const;
  std::string const& get_name() const;
  Lambda const& get_lambda() const;
  Builtin const& get_builtin() const;
  List const& get_list() const;
  Table const& get_table() const;
  bool& get_bool();
  double& get_double();
  int64_t& get_int();
  std::string& get_string();
  std::string& get_name();
  Lambda& get_lambda();
  Builtin& get_builtin();
  List& get_list();
  Table& get_table();
  bool truthy() const;

private:
  Type _type{Type::Null};
  std::variant<
      bool,
      double,
      int64_t,
      std::string,
      std::shared_ptr<Lambda>,
      std::shared_ptr<Builtin>,
      std::shared_ptr<List>,
      std::shared_ptr<Table>>
      _value;
};

bool operator==(Expr const& lhs, Expr const& rhs);
bool operator!=(Expr const& lhs, Expr const& rhs);
std::ostream& operator<<(std::ostream& stream, Expr const& expr);

bool IsQuote(Expr const& expr);
Expr Unquote(Expr const& expr);

} // namespace afct
