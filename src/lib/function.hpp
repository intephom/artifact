#pragma once

#include "env.hpp"
#include "expr.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <functional>

#define AFCT_ARG_CHECK(__predicate, __message) \
  AFCT_CHECK( \
      __predicate, fmt::format("{} but got {}", __message, afct::Expr{args}));

namespace afct {

struct Lambda
{
  List params;
  Expr body;
  std::shared_ptr<Env> env;
};

bool operator==(Lambda const& lhs, Lambda const& rhs);
bool operator!=(Lambda const& lhs, Lambda const& rhs);
std::ostream& operator<<(std::ostream& stream, Lambda const& lambda);

class INativeFunction
{
public:
  virtual ~INativeFunction() = default;
  virtual Expr call(List& args, std::shared_ptr<Env> env) = 0;
};

class StdFunction : public INativeFunction
{
public:
  StdFunction(std::function<Expr(List&, std::shared_ptr<Env>)> function);
  Expr call(List& args, std::shared_ptr<Env> env) final;

private:
  std::function<Expr(List&, std::shared_ptr<Env>)> _function;
};

Expr StdFunctionToExpr(
    std::string name,
    std::function<Expr(List&, std::shared_ptr<Env>)> function);

struct Builtin
{
  std::string name;
  std::shared_ptr<INativeFunction> function;
};

bool operator==(Builtin const& lhs, Builtin const& rhs);
bool operator!=(Builtin const& lhs, Builtin const& rhs);
std::ostream& operator<<(std::ostream& stream, Builtin const& builtin);

template<class T, class... Args>
Expr NativeFunctionToExpr(std::string name, Args... args)
{
  return Expr{Builtin{
      std::move(name), std::make_shared<T>(std::forward<Args>(args)...)}};
}

Expr Call(Expr const& expr, List& args, std::shared_ptr<Env> env);

} // namespace afct
