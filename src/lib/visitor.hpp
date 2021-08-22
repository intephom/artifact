#pragma once

#include "expr.hpp"

namespace afct {

class IVisitor
{
public:
  virtual ~IVisitor() = default;

  virtual void null_value() = 0;
  virtual void bool_value(bool value) = 0;
  virtual void double_value(double value) = 0;
  virtual void int_value(int64_t value) = 0;
  virtual void string_value(std::string const& value) = 0;
  virtual void name_value(std::string const& value) = 0;
  virtual void lambda_value(Lambda const& value) = 0;
  virtual void builtin_value(Builtin const& value) = 0;
  virtual void start_list(size_t size) = 0;
  virtual void end_list() = 0;
  virtual void start_table(size_t size) = 0;
  virtual void start_key() = 0;
  virtual void end_key() = 0;
  virtual void end_table() = 0;
};

void Visit(Expr const& expr, IVisitor* visitor);

} // namespace afct
