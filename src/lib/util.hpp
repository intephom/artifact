#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#define AFCT_ERROR(__message) throw afct::Exception(__message);

// clang-format off
#define AFCT_CHECK(__predicate, __message) \
  do \
  { \
    if (!(__predicate)) [[unlikely]] \
      AFCT_ERROR(__message); \
  } while (false);
// clang-format on

namespace afct {

class Exception : public std::runtime_error
{
public:
  Exception(std::string what) noexcept;
};

std::vector<std::string> Split(
    std::string const& delimiter, std::string const& str);

} // namespace afct
