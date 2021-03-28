#include "builder.hpp"

#include "eval.hpp"
#include "parse.hpp"
#include "util.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

BOOST_AUTO_TEST_CASE(builder_types)
{
  auto builder = Builder();
  builder.start_list();
  builder.null_value();
  builder.bool_value(true);
  builder.bool_value(false);
  builder.double_value(2.7);
  builder.int_value(2);
  builder.string_value("hello");
  builder.name_value("lambda");
  builder.expr_value(Parse("(+ 1 2)"));
  builder.end_list();

  auto code = R"((null true false 2.7 2 "hello" lambda (+ 1 2)))";

  BOOST_TEST(builder.get_string() == code);
}

BOOST_AUTO_TEST_CASE(builder_lists)
{
  auto builder = Builder();
  builder.start_list();
  builder.start_list();
  builder.name_value("+");
  builder.int_value(1);
  builder.start_list();
  builder.name_value("quote");
  builder.start_list();
  builder.name_value("+");
  builder.int_value(2);
  builder.int_value(3);
  builder.end_list();
  builder.end_list();
  builder.int_value(4);
  builder.end_list();
  builder.end_list();

  auto code = R"(((+ 1 '(+ 2 3) 4)))";

  BOOST_TEST(builder.get_string() == code);
}

BOOST_AUTO_TEST_CASE(builder_tables)
{
  auto builder = Builder();
  builder.start_list();
  builder.start_table();
  builder.bool_key(true);
  builder.start_table();
  builder.double_key(2.7);
  builder.start_table();
  builder.int_key(5);
  builder.start_table();
  builder.string_key("hello");
  builder.start_table();
  builder.name_key("lambda");
  builder.bool_value(true);
  builder.expr_key(EvalSimple("(quote expr)"));
  builder.bool_value(false);
  builder.end_table();
  builder.end_table();
  builder.end_table();
  builder.end_table();
  builder.end_table();
  builder.end_list();

  auto code_ordered =
      R"((#(true #(2.7 #(5 #("hello" #(lambda true expr false)))))))";
  auto code_unordered =
      R"((#(true #(2.7 #(5 #("hello" #(expr false lambda true)))))))";

  auto code = builder.get_string();
  bool ok = code == code_ordered || code == code_unordered;
  BOOST_TEST(ok);
}
