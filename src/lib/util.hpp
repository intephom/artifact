#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define AFCT_ERROR(message) \
  do \
  { \
    std::ostringstream stream; \
    stream << message; \
    throw std::runtime_error(stream.str()); \
  } while(false);

namespace afct {

std::vector<std::string> Split(
    std::string const& delimiter, std::string const& str);
}
