#include "builder.hpp"

#include "util.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>

namespace afct {

bool Builder::null_value()
{
  return push(Expr{});
}

bool Builder::bool_value(bool value)
{
  return push(Expr{value});
}

bool Builder::double_value(double value)
{
  return push(Expr{value});
}

bool Builder::int_value(int64_t value)
{
  return push(Expr{value});
}

bool Builder::string_value(std::string value)
{
  return push(Expr{String{std::move(value)}});
}

bool Builder::name_value(std::string value)
{
  return push(Expr{Name{std::move(value)}});
}

bool Builder::expr_value(Expr value)
{
  bool is_list = value.is_list();
  bool is_table = value.is_table();

  auto result = push(std::move(value));

  if (is_list)
    end_list();
  if (is_table)
    end_table();

  return result;
}

void Builder::bool_key(bool key)
{
  _key = Expr{key};
}

void Builder::double_key(double key)
{
  _key = Expr{key};
}

void Builder::int_key(int64_t key)
{
  _key = Expr{key};
}

void Builder::string_key(std::string key)
{
  _key = Expr{String{std::move(key)}};
}

void Builder::name_key(std::string key)
{
  _key = Expr{Name{std::move(key)}};
}

void Builder::expr_key(Expr key)
{
  _key = std::move(key);
}

bool Builder::start_list()
{
  auto expr = Expr{List{}};
  return push(std::move(expr));
}

bool Builder::end_list()
{
  if (_stack.empty() || !_stack.back().is_list())
    return false;

  _stack.pop_back();
  return true;
}

bool Builder::start_table()
{
  auto expr = Expr{Table{}};
  return push(std::move(expr));
}

bool Builder::end_table()
{
  if (_stack.empty() || !_stack.back().is_table())
    return false;

  _stack.pop_back();
  return true;
}

std::string Builder::get_string() const
{
  return fmt::format("{}", _root);
}

Expr Builder::get_expr() const
{
  return _root;
}

bool Builder::push(Expr expr)
{
  if (_stack.empty())
  {
    _root = expr;
    _stack.push_back(_root);
    return true;
  }

  auto& current = _stack.back();
  if (current.is_list())
  {
    current.get_list().push_back(expr);
  }
  else if (current.is_table())
  {
    if (_key.get_type() == Type::Null)
      return false;

    current.get_table()[_key] = expr;
    _key = Expr();
  }
  else
  {
    return false;
  }

  if (expr.is_list() || expr.is_table())
    _stack.push_back(expr);

  return true;
}

} // namespace afct
