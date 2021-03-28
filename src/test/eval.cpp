#include "eval.hpp"

#include "parse.hpp"
#include "util.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

BOOST_AUTO_TEST_CASE(eval_nothing)
{
  BOOST_TEST(EvalSimple("") == Expr::FromNull());
}

BOOST_AUTO_TEST_CASE(eval_types)
{
  BOOST_TEST(EvalSimple("true") == Expr::FromBool(true));
  BOOST_TEST(EvalSimple("false") == Expr::FromBool(false));
  BOOST_TEST(EvalSimple("2.7") == Expr::FromDouble(2.7));
  BOOST_TEST(EvalSimple("2") == Expr::FromInt(2));
  BOOST_TEST(EvalSimple(R"("string")") == Expr::FromString("string"));
  BOOST_TEST(EvalSimple("'name") == Expr::FromName("name"));
  BOOST_TEST(EvalSimple("car").is_function() == true);
  BOOST_TEST(EvalSimple("'(1 2)").is_list() == true);
  BOOST_TEST(EvalSimple("#(1 2 3 4)").is_table() == true);
}

BOOST_AUTO_TEST_CASE(eval_builtin)
{
  auto code = R"(
    (begin
      (define f
        (lambda (x)
          (if (= x 2)
            (+ x 10)
            x)))
      (apply f '(2))))";
  BOOST_TEST(EvalSimple(code) == Expr::FromInt(12));
}

BOOST_AUTO_TEST_CASE(eval_stdlib_math)
{
  BOOST_TEST(EvalSimple("(= 1 1)") == Expr::FromBool(true));
  BOOST_TEST(EvalSimple("(+ 1 1)") == Expr::FromInt(2));
  BOOST_TEST(EvalSimple("(- 1 1)") == Expr::FromInt(0));
  BOOST_TEST(EvalSimple("(* 2 3)") == Expr::FromInt(6));
  BOOST_TEST(EvalSimple("(/ 12 2)") == Expr::FromInt(6));
  BOOST_TEST(EvalSimple("(< 1 2)") == Expr::FromBool(true));
  BOOST_TEST(EvalSimple("(< 2 1)") == Expr::FromBool(false));
  BOOST_TEST(EvalSimple("(> 1 2)") == Expr::FromBool(false));
  BOOST_TEST(EvalSimple("(> 2 1)") == Expr::FromBool(true));
  BOOST_TEST(EvalSimple("(and false true)") == Expr::FromBool(false));
  BOOST_TEST(EvalSimple("(and true true)") == Expr::FromBool(true));
  BOOST_TEST(EvalSimple("(or false true)") == Expr::FromBool(true));
  BOOST_TEST(EvalSimple("(or false false)") == Expr::FromBool(false));
  BOOST_TEST(EvalSimple("(not false)") == Expr::FromBool(true));
  BOOST_TEST(EvalSimple("(not true)") == Expr::FromBool(false));
}

BOOST_AUTO_TEST_CASE(eval_stdlib_structure)
{
  BOOST_TEST(EvalSimple("(list 1 2)") == EvalSimple("'(1 2)"));
  BOOST_TEST(
      EvalSimple("(table 1 2 3 4)") == EvalSimple("#(1 2 3 4)"));
  BOOST_TEST(EvalSimple("(length '(1 2))") == Expr::FromInt(2));
  BOOST_TEST(
      EvalSimple("(append '(1 2 3 4) 5)") ==
      EvalSimple("'(1 2 3 4 5)"));
  BOOST_TEST(
      EvalSimple("(cons '(1 2) '(3 4))") == Parse("((1 2) (3 4))"));
  BOOST_TEST(EvalSimple("(car '(1 2 3))") == Expr::FromInt(1));
  BOOST_TEST(EvalSimple("(cdr '(1 2 3))") == Parse("(2 3)"));
}

BOOST_AUTO_TEST_CASE(eval_stdlib_types)
{
  BOOST_TEST(
      EvalSimple(R"((cat  "hello " "world"))") ==
      Parse(R"("hello world")"));
  BOOST_TEST(EvalSimple("(get #(1 2 3 4) 1)") == Expr::FromInt(2));
  BOOST_TEST(
      EvalSimple(
          "(begin (define a #(1 2 3 4)) (set! a 3 5) (get a 3))") ==
      Expr::FromInt(5));
  BOOST_TEST(EvalSimple("(bool 2)") == Expr::FromBool(true));
  BOOST_TEST(EvalSimple("(double 2)") == Expr::FromDouble(2));
  BOOST_TEST(EvalSimple("(int 2.7)") == Expr::FromInt(2));
  BOOST_TEST(EvalSimple("(string 2.7)") == Expr::FromString("2.7"));
}

BOOST_AUTO_TEST_CASE(eval_stdlib_functional)
{
  BOOST_TEST(
      EvalSimple("(apply (lambda (x y) (* x y)) '(2 3))") ==
      Expr::FromInt(6));
  BOOST_TEST(
      EvalSimple("(map (lambda (x) (* x 2)) '(1 2 3))") ==
      Parse("(2 4 6)"));
  BOOST_TEST(
      EvalSimple("(filter (lambda (x) (= x 2)) '(1 2 3))") ==
      Parse("(2)"));
}
