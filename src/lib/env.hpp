#pragma once

#include "expr.hpp"
#include "util.hpp"
#include <string>
#include <unordered_map>

namespace afct {

class Env
{
public:
  explicit Env(Env* outer = nullptr);
  void set(std::string key, Expr value);
  Expr find(std::string const& key) const;

private:
  std::unordered_map<std::string, Expr> _local;
  Env* _outer;
};

} // namespace afct
