#include "expr.hpp"
#include "function.hpp"
#include "util.hpp"
#include <functional>

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
    case Type::Function: return std::hash<std::string>()(expr.get_function()->name());
    case Type::List: AFCT_ERROR("List not usable as table key");
    case Type::Table: AFCT_ERROR("Table not usable as table key");
    default: AFCT_ERROR("Type not covered in std::hash");
  }
}

}

namespace afct {

Expr::Expr(Type type)
  : _type(type)
{}

Expr Expr::FromNull()
{
  Expr expr(Type::Null);
  return expr;
}

Expr Expr::FromBool(bool b)
{
  Expr expr(Type::Bool);
  expr._b = b;
  return expr;
}

Expr Expr::FromDouble(double d)
{
  Expr expr(Type::Double);
  expr._d = d;
  return expr;
}

Expr Expr::FromInt(int64_t i)
{
  Expr expr(Type::Int);
  expr._i = i;
  return expr;
}

Expr Expr::FromString(std::string s)
{
  Expr expr(Type::String);
  expr._sn = std::move(s);
  return expr;
}

Expr Expr::FromName(std::string n)
{
  Expr expr(Type::Name);
  expr._sn = std::move(n);
  return expr;
}

Expr Expr::FromFunction(std::shared_ptr<IFunction> f)
{
  Expr expr(Type::Function);
  expr._f = std::move(f);
  return expr;
}

Expr Expr::FromList(std::shared_ptr<List> l)
{
  Expr expr(Type::List);
  expr._l = std::move(l);
  return expr;
}

Expr Expr::FromTable(std::shared_ptr<Table> t)
{
  Expr expr(Type::Table);
  expr._t = std::move(t);
  return expr;
}

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

  return _b;
}

double Expr::get_double() const
{
  if (!is_double())
    AFCT_ERROR("Expression " << *this << " is not a double");

  return _d;
}

int64_t Expr::get_int() const
{
  if (!is_int())
    AFCT_ERROR("Expression " << *this << " is not a int");

  return _i;
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

std::string Expr::get_string() const
{
  if (!is_string())
    AFCT_ERROR("Expression " << *this << " is not a string");

  return _sn;
}

std::string Expr::get_name() const
{
  if (!is_name())
    AFCT_ERROR("Expression " << *this << " is not a name");

  return _sn;
}

std::shared_ptr<IFunction> Expr::get_function() const
{
  if (!is_function())
    AFCT_ERROR("Expression " << *this << " is not a function");

  return _f;
}

std::shared_ptr<List> Expr::get_list() const
{
  if (!is_list())
    AFCT_ERROR("Expression " << *this << " is not a list");

  return _l;
}

std::shared_ptr<Table> Expr::get_table() const
{
  if (!is_table())
    AFCT_ERROR("Expression " << *this << " is not a table");

  return _t;
}

bool Expr::truthy() const
{
  return _b
    || _d
    || _i
    || !_sn.empty()
    || _f.get()
    || (_l && !_l->empty())
    || (_t && !_t->empty());
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
    case Type::Function: return lhs.get_function()->name() == rhs.get_function()->name();
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

      std::ostringstream substream;
      substream << "(";
      for (auto const& subexpr : list)
        substream << subexpr << " ";
      substream.seekp(-1, std::ios_base::end);
      substream << ")";
      return stream << substream.str();
    }
  }
  else if (type == Type::Table)
  {
    auto const& table = *expr.get_table();
    if (table.empty())
      return stream << "#()";

    stream << "#(";
    std::ostringstream substream;
    for (auto const& pair : table)
      substream << pair.first << " " << pair.second << " ";
    substream.seekp(-1, std::ios_base::end);
    substream << ")";
    return stream << substream.str();
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

}
