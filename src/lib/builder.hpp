#pragma once

#include "expr.hpp"

namespace afct {

class Builder
{
public:
  bool null_value();
  bool bool_value(bool value);
  bool double_value(double value);
  bool int_value(int64_t value);
  bool string_value(std::string value);
  bool name_value(std::string value);
  bool expr_value(Expr value);
  void bool_key(bool key);
  void double_key(double key);
  void int_key(int64_t key);
  void string_key(std::string key);
  void name_key(std::string key);
  void expr_key(Expr key);
  bool start_list();
  bool end_list();
  bool start_table();
  bool end_table();
  std::string get_string() const;
  Expr get_expr() const;

private:
  bool push(Expr expr);

  std::vector<Expr> _stack;
  Expr _root;
  Expr _key;
};

} // namespace afct
