#include "eval.hpp"
#include "prelude.hpp"
#include <iostream>

void Repl()
{
  auto env = afct::Prelude();
  while (true)
  {
    std::cout << "> ";
    std::string input;
    if (!std::getline(std::cin, input))
      break;

    try
    {
      auto new_env = env;
      auto result = afct::Eval(input, new_env);
      env = new_env;
      std::cout << result << std::endl;
    }
    catch (std::runtime_error const& e)
    {
      std::cout << e.what() << std::endl;
    }
  }
}

int main(int argc, char* argv[])
{
  if (argc > 2)
  {
    std::cerr << "Usage: " << argv[0] << " [<file>]" << std::endl;
    return 1;
  }

  if (argc == 2)
  {
    auto env = afct::Prelude();
    afct::Eval(std::filesystem::path(argv[1]), env);
  }
  else
  {
    Repl();
  }

  return 0;
}
