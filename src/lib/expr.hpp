#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace afct {

class Expr;

}

namespace std {

template<>
struct hash<afct::Expr>
{
  std::size_t operator()(afct::Expr const& expr) const;
};

} // namespace std

namespace afct {

enum class Type
{
  Unknown,
  Null,
  Bool,
  Double,
  Int,
  String,
  Name,
  Function,
  List,
  Table
};

class Expr;
class IFunction;
using List = std::vector<Expr>;
using Table = std::unordered_map<Expr, Expr>;

class Expr
{
public:
  explicit Expr(Type type = Type::Unknown);
  static Expr FromNull();
  static Expr FromBool(bool b);
  static Expr FromDouble(double d);
  static Expr FromInt(int64_t i);
  static Expr FromString(std::string s);
  static Expr FromName(std::string n);
  static Expr FromFunction(std::shared_ptr<IFunction> f);
  static Expr FromList(std::shared_ptr<List> l);
  static Expr FromTable(std::shared_ptr<Table> t);
  bool is_null() const;
  bool is_bool() const;
  bool is_double() const;
  bool is_int() const;
  bool is_numeric() const;
  bool is_string() const;
  bool is_name() const;
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
  std::shared_ptr<IFunction> const& get_function() const;
  std::shared_ptr<List> const& get_list() const;
  std::shared_ptr<Table> const& get_table() const;
  bool truthy() const;

private:
  Type _type{Type::Unknown};

  bool _b{false};
  double _d{0.0};
  int64_t _i{0};
  std::string _sn;
  std::shared_ptr<IFunction> _f;
  std::shared_ptr<List> _l;
  std::shared_ptr<Table> _t;
};

bool operator==(Expr const& lhs, Expr const& rhs);
bool operator!=(Expr const& lhs, Expr const& rhs);
std::ostream& operator<<(std::ostream& stream, Expr const& expr);

bool IsQuote(Expr const& expr);
Expr Unquote(Expr const& expr);

} // namespace afct
