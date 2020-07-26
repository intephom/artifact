#include "env.hpp"

namespace afct {

Env::Env(Env* outer) : _outer(outer)
{}

void Env::set(std::string key, Expr value)
{
  _local[std::move(key)] = std::move(value);
}

Expr Env::find(std::string const& key) const
{
  auto it = _local.find(key);
  if (it != _local.end())
    return it->second;
  if (!_outer)
    AFCT_ERROR("Undefined identifier " << key);
  return _outer->find(key);
}

}
