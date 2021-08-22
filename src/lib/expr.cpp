#include "expr.hpp"

#include "function.hpp"
#include "util.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <functional>
#include <sstream>

namespace {

template<class T, class F>
std::ostream& FormatContainer(
    std::ostream& stream,
    std::string const& start,
    std::string const& end,
    std::string const& sep,
    T const& container,
    F formatter)
{
  std::ostringstream substream;

  substream << start;
  for (auto const& item : container)
  {
    formatter(substream, item);
    substream << sep;
  }
  substream.seekp(-1, std::ios_base::end);
  substream << end;

  return stream << substream.str();
}

} // namespace

namespace std {

size_t hash<afct::Expr>::operator()(afct::Expr const& expr) const
{
  using namespace afct;

  switch (expr.get_type())
  {
  case Type::Null: AFCT_ERROR("Null not hashable");
  case Type::Bool: return std::hash<bool>()(expr.get_bool());
  case Type::Double: return std::hash<double>()(expr.get_double());
  case Type::Int: return std::hash<int64_t>()(expr.get_int());
  case Type::String: return std::hash<std::string>()(expr.get_string());
  case Type::Name: return std::hash<std::string>()(expr.get_name());
  case Type::Lambda: AFCT_ERROR("Lambda not hashable");
  case Type::Builtin: AFCT_ERROR("Builtin not hashable");
  case Type::List: AFCT_ERROR("List not hashable");
  case Type::Table: AFCT_ERROR("Table not hashable");
  default: AFCT_ERROR("Type not covered in std::hash");
  }
}

} // namespace std

namespace afct {

Expr::Expr() : _type(Type::Null)
{}

Expr::Expr(Type type) : _type(type)
{}

Expr::Expr(bool b) : _type(Type::Bool), _value(b)
{}

Expr::Expr(double d) : _type(Type::Double), _value(d)
{}

Expr::Expr(int32_t i) : _type(Type::Int), _value(static_cast<int64_t>(i))
{}

Expr::Expr(int64_t i) : _type(Type::Int), _value(i)
{}

Expr::Expr(String s) : _type(Type::String), _value(std::move(s.s))
{}

Expr::Expr(Name n) : _type(Type::Name), _value(std::move(n.n))
{}

Expr::Expr(Lambda l)
  : _type(Type::Lambda), _value(std::make_shared<Lambda>(std::move(l)))
{}

Expr::Expr(Builtin b)
  : _type(Type::Builtin), _value(std::make_shared<Builtin>(std::move(b)))
{}

Expr::Expr(List l)
  : _type(Type::List), _value(std::make_shared<List>(std::move(l)))
{}

Expr::Expr(Table t)
  : _type(Type::Table), _value(std::make_shared<Table>(std::move(t)))
{}

bool Expr::is_null() const
{
  return _type == Type::Null;
}

bool Expr::is_bool() const
{
  return _type == Type::Bool;
}

bool Expr::is_double() const
{
  return _type == Type::Double;
}

bool Expr::is_int() const
{
  return _type == Type::Int;
}

bool Expr::is_numeric() const
{
  return _type == Type::Int || _type == Type::Double;
}

bool Expr::is_string() const
{
  return _type == Type::String;
}

bool Expr::is_name() const
{
  return _type == Type::Name;
}

bool Expr::is_lambda() const
{
  return _type == Type::Lambda;
}

bool Expr::is_builtin() const
{
  return _type == Type::Builtin;
}

bool Expr::is_function() const
{
  return _type == Type::Lambda || _type == Type::Builtin;
}

bool Expr::is_list() const
{
  return _type == Type::List;
}

bool Expr::is_table() const
{
  return _type == Type::Table;
}

Type Expr::get_type() const
{
  return _type;
}

bool Expr::get_bool() const
{
  AFCT_CHECK(is_bool(), fmt::format("Expression {} is not a bool", *this));
  return std::get<bool>(_value);
}

double Expr::get_double() const
{
  AFCT_CHECK(is_double(), fmt::format("Expression {} is not a double", *this));
  return std::get<double>(_value);
}

int64_t Expr::get_int() const
{
  AFCT_CHECK(is_int(), fmt::format("Expression {} is not an int", *this));
  return std::get<int64_t>(_value);
}

double Expr::get_numeric() const
{
  if (is_double())
    return get_double();
  else if (is_int())
    return get_int();

  AFCT_ERROR(fmt::format("Expression {} is not numeric", *this));
}

std::string const& Expr::get_string() const
{
  AFCT_CHECK(is_string(), fmt::format("Expression {} is not a string", *this));
  return std::get<std::string>(_value);
}

std::string const& Expr::get_name() const
{
  AFCT_CHECK(is_name(), fmt::format("Expression {} is not a name", *this));
  return std::get<std::string>(_value);
}

Lambda const& Expr::get_lambda() const
{
  AFCT_CHECK(is_lambda(), fmt::format("Expression {} is not a lambda", *this));
  return *std::get<std::shared_ptr<Lambda>>(_value);
}

Builtin const& Expr::get_builtin() const
{
  AFCT_CHECK(
      is_builtin(), fmt::format("Expression {} is not a builtin", *this));
  return *std::get<std::shared_ptr<Builtin>>(_value);
}

List const& Expr::get_list() const
{
  AFCT_CHECK(is_list(), fmt::format("Expression {} is not a list", *this));
  return *std::get<std::shared_ptr<List>>(_value);
}

Table const& Expr::get_table() const
{
  AFCT_CHECK(is_table(), fmt::format("Expression {} is not a table", *this));
  return *std::get<std::shared_ptr<Table>>(_value);
}

bool& Expr::get_bool()
{
  AFCT_CHECK(is_bool(), fmt::format("Expression {} is not a bool", *this));
  return std::get<bool>(_value);
}

double& Expr::get_double()
{
  AFCT_CHECK(is_double(), fmt::format("Expression {} is not a double", *this));
  return std::get<double>(_value);
}

int64_t& Expr::get_int()
{
  AFCT_CHECK(is_int(), fmt::format("Expression {} is not an int", *this));
  return std::get<int64_t>(_value);
}

std::string& Expr::get_string()
{
  AFCT_CHECK(is_string(), fmt::format("Expression {} is not a string", *this));
  return std::get<std::string>(_value);
}

std::string& Expr::get_name()
{
  AFCT_CHECK(is_name(), fmt::format("Expression {} is not a name", *this));
  return std::get<std::string>(_value);
}

Lambda& Expr::get_lambda()
{
  AFCT_CHECK(is_lambda(), fmt::format("Expression {} is not a lambda", *this));
  return *std::get<std::shared_ptr<Lambda>>(_value);
}

Builtin& Expr::get_builtin()
{
  AFCT_CHECK(
      is_builtin(), fmt::format("Expression {} is not a builtin", *this));
  return *std::get<std::shared_ptr<Builtin>>(_value);
}

List& Expr::get_list()
{
  AFCT_CHECK(is_list(), fmt::format("Expression {} is not a list", *this));
  return *std::get<std::shared_ptr<List>>(_value);
}

Table& Expr::get_table()
{
  AFCT_CHECK(is_table(), fmt::format("Expression {} is not a table", *this));
  return *std::get<std::shared_ptr<Table>>(_value);
}

bool Expr::truthy() const
{
  switch (_type)
  {
  case Type::Null: return false;
  case Type::Bool: return get_bool();
  case Type::Double: return get_double() != 0.0;
  case Type::Int: return get_int() != 0;
  case Type::String: return !get_string().empty();
  case Type::Name: return !get_name().empty();
  case Type::Lambda: return true;
  case Type::Builtin: return true;
  case Type::List: return !get_list().empty();
  case Type::Table: return !get_table().empty();
  default: AFCT_ERROR("Type not covered truthy()");
  }
}

bool operator==(Expr const& lhs, Expr const& rhs)
{
  if (lhs.is_numeric() && rhs.is_numeric())
    return lhs.get_numeric() == rhs.get_numeric();

  if (lhs.get_type() != rhs.get_type())
    return false;

  switch (lhs.get_type())
  {
  case Type::Null: return rhs.get_type() == Type::Null;
  case Type::Bool: return lhs.get_bool() == rhs.get_bool();
  case Type::String: return lhs.get_string() == rhs.get_string();
  case Type::Name: return lhs.get_name() == rhs.get_name();
  case Type::Lambda: return lhs.get_lambda() == rhs.get_lambda();
  case Type::Builtin: return lhs.get_builtin() == rhs.get_builtin();
  case Type::List: return lhs.get_list() == rhs.get_list();
  case Type::Table: return lhs.get_table() == rhs.get_table();
  default: AFCT_ERROR("Type not covered in ==");
  }
}

bool operator!=(Expr const& lhs, Expr const& rhs)
{
  return !(rhs == lhs);
}

std::ostream& operator<<(std::ostream& stream, Expr const& expr)
{
  auto type = expr.get_type();

  if (type == Type::Null)
    return stream << "null";
  else if (type == Type::Bool)
    return stream << (expr.get_bool() ? "true" : "false");
  else if (type == Type::Double)
    return stream << expr.get_double();
  else if (type == Type::Int)
    return stream << expr.get_int();
  else if (type == Type::String)
    return stream << '"' << expr.get_string() << '"';
  else if (type == Type::Name)
    return stream << expr.get_name();
  else if (type == Type::Lambda)
    return stream << expr.get_lambda();
  else if (type == Type::Builtin)
    return stream << expr.get_builtin();
  else if (type == Type::List)
  {
    if (IsQuote(expr))
    {
      return stream << "'" << Unquote(expr);
    }
    else
    {
      auto const& list = expr.get_list();

      if (list.empty())
        return stream << "()";

      return FormatContainer(
          stream,
          "(",
          ")",
          " ",
          list,
          [](std::ostream& stream, auto const& element) { stream << element; });
    }
  }
  else if (type == Type::Table)
  {
    auto const& table = expr.get_table();

    if (table.empty())
      return stream << "#()";

    return FormatContainer(
        stream,
        "#(",
        ")",
        " ",
        table,
        [](std::ostream& stream, auto const& pair) {
          stream << pair.first << " " << pair.second;
        });
  }
  else
    return stream << "?";
}

bool IsQuote(Expr const& expr)
{
  if (!expr.is_list())
    return false;

  auto const& list = expr.get_list();
  if (list.empty())
    return false;

  auto const& first = list[0];
  if (!first.is_name())
    return false;

  auto const& name = first.get_name();
  return name == "quote" || name == "'";
}

Expr Unquote(Expr const& expr)
{
  if (!IsQuote(expr))
    return expr;

  auto const& list = expr.get_list();
  if (list.size() == 1)
    return expr;

  return list[1];
}

} // namespace afct
