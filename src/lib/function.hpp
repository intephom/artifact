#pragma once

#include "env.hpp"
#include "expr.hpp"
#include <functional>
#include <sstream>

#define AFCT_ARG_ERROR(__args, __message) \
  do \
  { \
    std::ostringstream __stream; \
    __stream << __message << " but got " << afct::Expr::FromList(__args); \
    throw std::runtime_error(__stream.str()); \
  } while (false);

namespace afct {

class IFunction
{
public:
  virtual ~IFunction() = default;
  virtual std::string name() const = 0;
  virtual Expr call(List const& args) = 0;
};

class Function : public IFunction
{
public:
  Function(std::string name, List params, Expr body, Env* env);
  std::string name() const final;
  Expr call(List const& args) final;

private:
  std::string _name;
  List _params;
  Expr _body;
  Env* _env;
};

class NativeFunction : public IFunction
{
public:
  NativeFunction(std::string name, std::function<Expr(List const&)> f);
  std::string name() const final;
  Expr call(List const& args) final;

private:
  std::string _name;
  std::function<Expr(List const&)> _f;
};

Expr NativeFunctionToExpr(std::string name, std::function<Expr(List const&)> f);

} // namespace afct
