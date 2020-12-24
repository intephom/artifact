#include "eval.hpp"
#include "expr.hpp"
#include "function.hpp"
#include "parse.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

class DummyFunction : public IFunction
{
public:
  std::string name() const final { return "dummy"; }
  Expr call(List const& args) final { return Expr::FromNull(); }
};

BOOST_AUTO_TEST_CASE(expr_is)
{
  BOOST_TEST(Expr::FromNull().is_null() == true);
  BOOST_TEST(Expr::FromBool(true).is_bool() == true);
  BOOST_TEST(Expr::FromDouble(2.7).is_double() == true);
  BOOST_TEST(Expr::FromInt(2).is_int() == true);
  BOOST_TEST(Expr::FromString("string").is_string() == true);
  BOOST_TEST(Expr::FromName("+").is_name() == true);
  BOOST_TEST(Expr::FromFunction(std::make_shared<DummyFunction>()).is_function() == true);
  BOOST_TEST(Parse("(1 2)").is_list() == true);
  BOOST_TEST(Eval(std::string("#(1 2)")).is_table() == true);
}

BOOST_AUTO_TEST_CASE(expr_get)
{
  BOOST_TEST(Expr::FromBool(true).get_bool() == true);
  BOOST_TEST(Expr::FromBool(false).get_bool() == false);
  BOOST_TEST(Expr::FromDouble(2.7).get_double() == 2.7);
  BOOST_TEST(Expr::FromInt(2).get_int() == 2);
  BOOST_TEST(Expr::FromString("string").get_string() == "string");
  BOOST_TEST(Expr::FromName("+").get_name() == "+");
  auto f = std::make_shared<DummyFunction>();
  BOOST_TEST(Expr::FromFunction(f).get_function() == f);
  auto l = List{Expr::FromInt(1), Expr::FromInt(2)};
  BOOST_TEST(*Parse("(1 2)").get_list() == l);
  auto t = Table({{Expr::FromInt(1), Expr::FromInt(2)}});
  BOOST_TEST(*Eval(std::string("#(1 2)")).get_table() == t);
}

BOOST_AUTO_TEST_CASE(expr_truthy)
{
  BOOST_TEST(Expr::FromNull().truthy() == false);
  BOOST_TEST(Expr::FromBool(true).truthy() == true);
  BOOST_TEST(Expr::FromBool(false).truthy() == false);
  BOOST_TEST(Expr::FromDouble(2.7).truthy() == true);
  BOOST_TEST(Expr::FromDouble(0.0).truthy() == false);
  BOOST_TEST(Expr::FromInt(2).truthy() == true);
  BOOST_TEST(Expr::FromInt(0).truthy() == false);
  BOOST_TEST(Expr::FromString("string").truthy() == true);
  BOOST_TEST(Expr::FromString("").truthy() == false);
  BOOST_TEST(Expr::FromName("+").truthy() == true);
  BOOST_TEST(Expr::FromName("").truthy() == false);
  BOOST_TEST(Expr::FromFunction(std::make_shared<DummyFunction>()).truthy() == true);
  BOOST_TEST(Expr::FromFunction(nullptr).truthy() == false);
  BOOST_TEST(Eval(std::string("'(1 2)")).truthy() == true);
  BOOST_TEST(Eval(std::string("'()")).truthy() == false);
  BOOST_TEST(Eval(std::string("#(1 2)")).truthy() == true);
  BOOST_TEST(Eval(std::string("#()")).truthy() == false);
}

BOOST_AUTO_TEST_CASE(expr_is_quote)
{
  BOOST_TEST(IsQuote(Parse("(quote (1 1))")) == true);
  BOOST_TEST(IsQuote(Parse("'(1 1)")) == true);
  BOOST_TEST(IsQuote(Parse("(print 27)")) == false);
  BOOST_TEST(IsQuote(Parse("#(1 2 3 4)")) == false);
}

BOOST_AUTO_TEST_CASE(expr_unquote)
{
  BOOST_TEST(Unquote(Parse("(quote (1 1))")) == Parse("(1 1)"));
  BOOST_TEST(Unquote(Parse("'(1 1)")) == Parse("(1 1)"));
  BOOST_TEST(Unquote(Parse("(print 27)")) == Parse("(print 27)"));
  BOOST_TEST(Unquote(Parse("#(1 '(1 2) 3 '(3 4))")) == Parse("#(1 '(1 2) 3 '(3 4))"));
}

BOOST_AUTO_TEST_CASE(expr_ostream)
{
  auto code = R"((null () true false 2. 2.7 .7 2 "hello" + (1 2 '(3 4)) (quote (1 2))))";
  auto sugared = R"((null () true false 2 2.7 0.7 2 "hello" + (1 2 '(3 4)) '(1 2)))";
  auto expr = Parse(code);
  std::ostringstream stream;
  stream << expr;

  BOOST_TEST(stream.str() == sugared);
}

BOOST_AUTO_TEST_CASE(expr_ostream_table)
{
  auto code = R"((table 1 2 3 4))";
  auto expr = Eval(std::string(code));
  auto sugared_ordered = R"(#(1 2 3 4))";
  auto sugared_unordered = R"(#(3 4 1 2))";
  std::ostringstream stream;
  stream << expr;

  bool ok = stream.str() == sugared_ordered || stream.str() == sugared_unordered;
  BOOST_TEST(ok);
}
