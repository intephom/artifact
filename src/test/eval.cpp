#include "eval.hpp"

#include "parse.hpp"
#include "util.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

BOOST_AUTO_TEST_CASE(eval_nothing)
{
  BOOST_TEST(EvalSimple("") == Expr{});
}

BOOST_AUTO_TEST_CASE(eval_types)
{
  BOOST_TEST(EvalSimple("true") == Expr{true});
  BOOST_TEST(EvalSimple("false") == Expr{false});
  BOOST_TEST(EvalSimple("2.7") == Expr{2.7});
  BOOST_TEST(EvalSimple("2") == Expr{2});
  BOOST_TEST(EvalSimple(R"("string")") == Expr{String{"string"}});
  BOOST_TEST(EvalSimple("'name") == Expr{Name{"name"}});
  BOOST_TEST(EvalSimple("car").is_function() == true);
  BOOST_TEST(EvalSimple("'(1 2)").is_list() == true);
  BOOST_TEST(EvalSimple("#(1 2 3 4)").is_table() == true);
  BOOST_TEST(EvalSimple("#(1 2 3 (+ 2 2))") == EvalSimple("#(1 2 3 4)"));
  BOOST_TEST(EvalSimple("#(1 2 1 3)") == EvalSimple("#(1 3)"));
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
  BOOST_TEST(EvalSimple(code) == Expr{12});
}

BOOST_AUTO_TEST_CASE(eval_stdlib_math)
{
  BOOST_TEST(EvalSimple("(= 1 1)") == Expr{true});
  BOOST_TEST(EvalSimple("(+ 1 1)") == Expr{2});
  BOOST_TEST(EvalSimple("(- 1 1)") == Expr{0});
  BOOST_TEST(EvalSimple("(* 2 3)") == Expr{6});
  BOOST_TEST(EvalSimple("(/ 12 2)") == Expr{6});
  BOOST_TEST(EvalSimple("(< 1 2)") == Expr{true});
  BOOST_TEST(EvalSimple("(< 2 1)") == Expr{false});
  BOOST_TEST(EvalSimple("(> 1 2)") == Expr{false});
  BOOST_TEST(EvalSimple("(> 2 1)") == Expr{true});
  BOOST_TEST(EvalSimple("(and false true)") == Expr{false});
  BOOST_TEST(EvalSimple("(and true true)") == Expr{true});
  BOOST_TEST(EvalSimple("(or false true)") == Expr{true});
  BOOST_TEST(EvalSimple("(or false false)") == Expr{false});
  BOOST_TEST(EvalSimple("(not false)") == Expr{true});
  BOOST_TEST(EvalSimple("(not true)") == Expr{false});
  BOOST_TEST(EvalSimple("(min '(3 6 5 1 4 8 7))") == Expr{1});
  BOOST_TEST(EvalSimple("(max '(3 6 5 1 4 8 7))") == Expr{8});
}

BOOST_AUTO_TEST_CASE(eval_stdlib_structure)
{
  BOOST_TEST(EvalSimple("(list 1 (+ 1 1))") == EvalSimple("'(1 2)"));
  BOOST_TEST(EvalSimple("(table 1 2 3 (+ 2 2))") == EvalSimple("#(1 2 3 4)"));
  BOOST_TEST(EvalSimple("(table 1 2 1 3)") == EvalSimple("#(1 3)"));
  BOOST_TEST(EvalSimple("(length '(1 2))") == Expr{2});
  BOOST_TEST(EvalSimple("(length #(1 2 3 4 5 6))") == Expr{3});
  BOOST_TEST(EvalSimple("(append '(1 2 3 4) 5)") == EvalSimple("'(1 2 3 4 5)"));
  BOOST_TEST(EvalSimple("(cons '(1 2) '(3 4))") == Parse("((1 2) (3 4))"));
  BOOST_TEST(EvalSimple("(car '(1 2 3))") == Expr{1});
  BOOST_TEST(EvalSimple("(cdr '(1 2 3))") == Parse("(2 3)"));
  BOOST_TEST(EvalSimple("(get #(1 2 3 4) 1)") == Expr{2});
  BOOST_TEST(
      EvalSimple("(begin (define a #(1 2 3 4)) (set! a 3 5) (get a 3))") ==
      Expr{5});
  auto keys = *EvalSimple("(keys #(1 2 3 4))").get_list();
  bool keys_ok =
      keys == List{Expr{1}, Expr{3}} || keys == List{Expr{3}, Expr{1}};
  BOOST_TEST(keys_ok);
  auto values = *EvalSimple("(values #(1 2 3 4))").get_list();
  bool values_ok =
      values == List{Expr{2}, Expr{4}} || values == List{Expr{4}, Expr{2}};
  BOOST_TEST(values_ok);
}

BOOST_AUTO_TEST_CASE(eval_stdlib_types)
{
  BOOST_TEST(
      EvalSimple(R"((cat  "hello " "world"))") == Parse(R"("hello world")"));
  BOOST_TEST(EvalSimple("(bool 2)") == Expr{true});
  BOOST_TEST(EvalSimple("(double 2)") == Expr{2.0});
  BOOST_TEST(EvalSimple("(int 2.7)") == Expr{2});
  BOOST_TEST(EvalSimple("(string 2.7)") == Expr{String{"2.7"}});
}

BOOST_AUTO_TEST_CASE(eval_stdlib_functional)
{
  BOOST_TEST(EvalSimple("(apply (lambda (x y) (* x y)) '(2 3))") == Expr{6});
  BOOST_TEST(
      EvalSimple("(map (lambda (x) (* x 2)) '(1 2 3))") == Parse("(2 4 6)"));
  BOOST_TEST(
      EvalSimple("(filter (lambda (x) (= x 2)) '(1 2 3))") == Parse("(2)"));
}
