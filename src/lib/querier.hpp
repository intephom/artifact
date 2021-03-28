#pragma once

#include "expr.hpp"
#include "util.hpp"

namespace afct {

class Querier
{
public:
  Querier();
  explicit Querier(Expr expr);

  bool get_list(std::string const& path, std::vector<Expr>& result) const;
  std::vector<Expr> get_list(std::string const& path) const;
  template<class T>
  bool get(std::string const& path, T& result) const;
  template<class T>
  T get(std::string const& path) const;

private:
  template<class T>
  bool get(Expr const& value, T& result) const;
  template<class T>
  bool get_int(Expr const& value, T& result) const;
  bool find(std::vector<std::string> const& elements, Expr& result) const;

  Expr _root;
};

} // namespace afct

namespace afct {

template<class T>
bool Querier::get(std::string const& path, T& result) const
{
  auto elements = Split("/", path);
  afct::Expr expr;
  if (!find(std::move(elements), expr))
    return false;
  return get<T>(expr, result);
}

template<class T>
T Querier::get(std::string const& path) const
{
  T result;
  if (!get<T>(path, result))
    AFCT_ERROR("Failed to get from " << path);
  return result;
}

template<class T>
bool Querier::get_int(Expr const& expr, T& result) const
{
  if (!expr.is_int())
    return false;

  auto i = expr.get_int();
  if (!std::numeric_limits<T>::is_signed && i < 0)
    return false;
  if (std::numeric_limits<T>::max() != std::numeric_limits<uint64_t>::max())
  {
    if (i > std::numeric_limits<T>::max())
      return false;
    else if (i < std::numeric_limits<T>::min())
      return false;
  }

  result = i;
  return true;
}

} // namespace afct
