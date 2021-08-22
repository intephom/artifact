#include "env.hpp"

#include "util.hpp"
#include <fmt/format.h>
#include <sstream>

namespace afct {

Env::Env(std::shared_ptr<Env> outer) : _outer(std::move(outer))
{}

void Env::set(std::string key, Expr value)
{
  _local[std::move(key)] = std::move(value);
}

std::optional<Expr> Env::find(std::string const& key) const
{
  auto it = _local.find(key);
  if (it != _local.end())
    return it->second;

  if (!_outer)
    return std::nullopt;
  return _outer->find(key);
}

} // namespace afct
