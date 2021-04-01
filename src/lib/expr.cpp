#include "expr.hpp"

#include "function.hpp"
#include "util.hpp"
#include <functional>

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

std::size_t hash<afct::Expr>::operator()(afct::Expr const& expr) const
{
  using namespace afct;

  switch (expr.get_type())
  {
  case Type::Null: AFCT_ERROR("Null not usable as table key");
  case Type::Bool: return std::hash<bool>()(expr.get_bool());
  case Type::Double: return std::hash<double>()(expr.get_double());
  case Type::Int: return std::hash<int64_t>()(expr.get_int());
  case Type::String: return std::hash<std::string>()(expr.get_string());
  case Type::Name: return std::hash<std::string>()(expr.get_name());
  case Type::Function: AFCT_ERROR("Function not usable as table key");
  case Type::List: AFCT_ERROR("List not usable as table key");
  case Type::Table: AFCT_ERROR("Table not usable as table key");
  default: AFCT_ERROR("Type not covered in std::hash");
  }
}

} // namespace std

namespace afct {

Expr::Expr(Type type) : _type(type)
{}

Expr::Expr() : _type(Type::Null)
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

Expr::Expr(std::shared_ptr<IFunction> f)
  : _type(Type::Function), _value(std::move(f))
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

bool Expr::is_function() const
{
  return _type == Type::Function;
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
  if (!is_bool())
    AFCT_ERROR("Expression " << *this << " is not a bool");

  return std::get<bool>(_value);
}

double Expr::get_double() const
{
  if (!is_double())
    AFCT_ERROR("Expression " << *this << " is not a double");

  return std::get<double>(_value);
}

int64_t Expr::get_int() const
{
  if (!is_int())
    AFCT_ERROR("Expression " << *this << " is not an int");

  return std::get<int64_t>(_value);
}

double Expr::get_numeric() const
{
  if (is_double())
    return get_double();
  else if (is_int())
    return get_int();
  else
    AFCT_ERROR("Expression " << *this << " is not numeric");
}

std::string const& Expr::get_string() const
{
  if (!is_string())
    AFCT_ERROR("Expression " << *this << " is not a string");

  return std::get<std::string>(_value);
}

std::string const& Expr::get_name() const
{
  if (!is_name())
    AFCT_ERROR("Expression " << *this << " is not a name");

  return std::get<std::string>(_value);
}

std::shared_ptr<IFunction> const& Expr::get_function() const
{
  if (!is_function())
    AFCT_ERROR("Expression " << *this << " is not a function");

  return std::get<std::shared_ptr<IFunction>>(_value);
}

std::shared_ptr<List> const& Expr::get_list() const
{
  if (!is_list())
    AFCT_ERROR("Expression " << *this << " is not a list");

  return std::get<std::shared_ptr<List>>(_value);
}

std::shared_ptr<Table> const& Expr::get_table() const
{
  if (!is_table())
    AFCT_ERROR("Expression " << *this << " is not a table");

  return std::get<std::shared_ptr<Table>>(_value);
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
  case Type::Function: return get_function() != nullptr;
  case Type::List: return !get_list()->empty();
  case Type::Table: return !get_table()->empty();
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
  case Type::Function: AFCT_ERROR("Cannot compare functions for equality");
  case Type::List: return *lhs.get_list() == *rhs.get_list();
  case Type::Table: return *lhs.get_table() == *rhs.get_table();
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
  else if (type == Type::Function)
    return stream << "<function " << expr.get_function()->name() << ">";
  else if (type == Type::List)
  {
    if (IsQuote(expr))
    {
      return stream << "'" << Unquote(expr);
    }
    else
    {
      auto const& list = *expr.get_list();

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
    auto const& table = *expr.get_table();

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

  auto const& list = *expr.get_list();
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

  auto const& list = *expr.get_list();
  if (list.size() == 1)
    return expr;

  return list[1];
}

} // namespace afct
