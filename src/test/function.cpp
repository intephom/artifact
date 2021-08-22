#include "lib/function.hpp"

#include "lib/eval.hpp"
#include "lib/parse.hpp"
#include "util.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

class TimesBy : public INativeFunction
{
public:
  explicit TimesBy(int n) : _n(n)
  {}

  Expr call(List& args, std::shared_ptr<Env>) final
  {
    AFCT_ARG_CHECK(
        args.size() == 1 && args[0].is_double(), "Expected 1 double arg");

    return Expr{args[0].get_double() * _n};
  }

private:
  int _n;
};

class IsDefined : public INativeFunction
{
public:
  Expr call(List& args, std::shared_ptr<Env> env) final
  {
    AFCT_ARG_CHECK(
        args.size() == 1 && args[0].is_string(), "Expected 1 string arg");

    return Expr{env->find(args[0].get_string()).has_value()};
  }
};

BOOST_AUTO_TEST_CASE(native_functions)
{
  auto env = Prelude();

  env->set("times2", NativeFunctionToExpr<TimesBy>("times2", 2));
  BOOST_TEST(Eval(std::string("(times2 2.0)"), env) == Expr{4.0});
  BOOST_CHECK_THROW(Eval(std::string("(times2 lambda)"), env), Exception);

  env->set("isdef", NativeFunctionToExpr<IsDefined>("isdef"));
  BOOST_TEST(Eval(std::string(R"((isdef "times2"))"), env) == Expr{true});
  BOOST_TEST(Eval(std::string(R"((isdef "times3"))"), env) == Expr{false});
}
