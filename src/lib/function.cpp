#include "function.hpp"

#include "eval.hpp"

namespace afct {

Function::Function(std::string name, List params, Expr body, Env* env)
  : _name(std::move(name))
  , _params(std::move(params))
  , _body(std::move(body))
  , _env(env)
{}

std::string Function::name() const
{
  return _name;
}

Expr Function::call(List const& args)
{
  Env env(_env);

  if (_params.size() != args.size())
    AFCT_ERROR(
        "Expected " << _params.size() << " args to " << _name << " but got "
                    << args.size());

  for (size_t i = 0; i < _params.size(); i++)
  {
    if (!_params[i].is_name())
      AFCT_ERROR("Param " << _params[i] << " is not a name");
    env.set(_params[i].get_name(), args[i]);
  }
  return Eval(_body, env);
}

NativeFunction::NativeFunction(
    std::string name, std::function<Expr(List const&)> f)
  : _name(std::move(name)), _f(f)
{}

std::string NativeFunction::name() const
{
  return _name;
}

Expr NativeFunction::call(List const& args)
{
  return _f(args);
}

Expr NativeFunctionToExpr(std::string name, std::function<Expr(List const&)> f)
{
  return Expr::FromFunction(
      std::make_shared<NativeFunction>(std::move(name), f));
}

} // namespace afct
