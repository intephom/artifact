#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define AFCT_ERROR(__message) \
  do \
  { \
    std::ostringstream __stream; \
    __stream << __message; \
    throw std::runtime_error(__stream.str()); \
  } while (false);

namespace afct {

std::vector<std::string> Split(
    std::string const& delimiter, std::string const& str);
}
