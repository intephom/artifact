#include "util.hpp"
#include <sstream>

namespace afct {

std::vector<std::string> Split(std::string const& delimiter, std::string const& str)
{
  std::vector<std::string> result;
  size_t start_pos = 0;
  while(start_pos < str.size())
  {
    size_t end_pos = str.find(delimiter, start_pos);
    result.push_back(str.substr(start_pos, end_pos - start_pos));
    if(end_pos == std::string::npos)
      break;
    start_pos = end_pos + delimiter.size();
  }
  return result;
}

}
