#pragma once

#include "expr.hpp"
#include "util.hpp"
#include <boost/container/flat_map.hpp>
#include <optional>
#include <string>

namespace afct {

class Env
{
public:
  Env(std::shared_ptr<Env> outer = nullptr);
  void set(std::string key, Expr value);
  std::optional<Expr> find(std::string const& key) const;

private:
  boost::container::flat_map<std::string, Expr> _local;
  std::shared_ptr<Env> _outer;
};

} // namespace afct
