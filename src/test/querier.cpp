#include "querier.hpp"

#include "eval.hpp"
#include "parse.hpp"
#include "util.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace afct;

BOOST_AUTO_TEST_CASE(querier_list)
{
  auto code = R"(#("onetwo" '(1 2) "threefour" '(3 4)))";
  auto querier = Querier(EvalSimple(code));
  // unquoted for us
  auto list = *Parse("(1 2)").get_list();

  BOOST_TEST(querier.get_list("onetwo") == list);
}

BOOST_AUTO_TEST_CASE(querier_table)
{
  auto code = R"(#("toplevel" #(1 2 3 4)))";
  auto querier = Querier(EvalSimple(code));
  auto list_ordered = *Parse("((1 2) (3 4))").get_list();
  auto list_unordered = *Parse("((3 4) (1 2))").get_list();
  auto list = querier.get_list("toplevel");

  // ordering of table pairs is not defined
  bool ok = list == list_ordered || list == list_unordered;
  BOOST_TEST(ok == true);
}

BOOST_AUTO_TEST_CASE(querier_depth)
{
  auto code = R"(#("toplevel" #("onetwo" '(1 2) "threefour" '(3 4))))";
  auto querier = Querier(EvalSimple(code));
  auto list = *Parse("(1 2)").get_list();

  BOOST_TEST(querier.get_list("toplevel/onetwo") == list);
}

BOOST_AUTO_TEST_CASE(querier_bool)
{
  auto code = R"(#("toplevel" #("one" true "two" false)))";
  auto querier = Querier(EvalSimple(code));

  BOOST_TEST(querier.get<bool>("toplevel/one") == true);
  BOOST_TEST(querier.get<bool>("toplevel/two") == false);
}

BOOST_AUTO_TEST_CASE(querier_double)
{
  auto code = R"(#("toplevel" #("one" 2.0 "two" 2)))";
  auto querier = Querier(EvalSimple(code));

  BOOST_TEST(querier.get<double>("toplevel/one") == 2);
  // ints converted as well
  BOOST_TEST(querier.get<double>("toplevel/two") == 2);
}

BOOST_AUTO_TEST_CASE(querier_ints)
{
  auto code = R"(
    #(
      "i8underflow" -129
      "i8min" -128
      "i8max" 127
      "i8overflow" 128

      "u8max" 255
      "u8overflow" 256

      "i16underflow" -32769
      "i16min" -32768
      "i16max" 32767
      "i16overflow" 32768

      "u16max" 65535
      "u16overflow" 65536

      "i32underflow" -2147483649
      "i32min" -2147483648
      "i32max" 2147483647
      "i32overflow" 2147483648

      "u32max" 4294967295
      "u32overflow" 4294967295

      "i64min" -9223372036854775808
      "i64max" 9223372036854775807))";
  auto querier = Querier(EvalSimple(code));

  BOOST_CHECK_THROW(querier.get<int8_t>("i8underflow"), std::runtime_error);
  BOOST_TEST(querier.get<int8_t>("i8min"));
  BOOST_TEST(querier.get<int8_t>("i8max"));
  BOOST_CHECK_THROW(querier.get<int8_t>("i8overflow"), std::runtime_error);
  BOOST_TEST(querier.get<uint8_t>("i8overflow"));
  BOOST_TEST(querier.get<uint8_t>("u8max"));

  BOOST_CHECK_THROW(querier.get<int16_t>("i16underflow"), std::runtime_error);
  BOOST_TEST(querier.get<int16_t>("i16min"));
  BOOST_TEST(querier.get<int16_t>("i16max"));
  BOOST_CHECK_THROW(querier.get<int16_t>("i16overflow"), std::runtime_error);
  BOOST_TEST(querier.get<uint16_t>("i16overflow"));
  BOOST_TEST(querier.get<uint16_t>("u16max"));

  BOOST_CHECK_THROW(querier.get<int32_t>("i32underflow"), std::runtime_error);
  BOOST_TEST(querier.get<int32_t>("i32min"));
  BOOST_TEST(querier.get<int32_t>("i32max"));
  BOOST_CHECK_THROW(querier.get<int32_t>("i32overflow"), std::runtime_error);
  BOOST_TEST(querier.get<uint32_t>("i32overflow"));
  BOOST_TEST(querier.get<uint32_t>("u32max"));

  BOOST_TEST(querier.get<int64_t>("i64min"));
  BOOST_TEST(querier.get<int64_t>("i64max"));
}

BOOST_AUTO_TEST_CASE(querier_types)
{
  auto code = R"(
    #(
      "bool" true
      "double" 2.7
      "i8" 127
      "u8" 255
      "i16" 32767
      "u16" 65535
      "i32" 2147483647
      "u32" 4294967295
      "i64" 9223372036854775807
      "string" "hello"
      "name" 'lambda))";
  auto querier = Querier(EvalSimple(code));

  BOOST_TEST(querier.get<bool>("bool") == true);
  BOOST_TEST(querier.get<double>("double") == 2.7);
  BOOST_TEST(querier.get<int8_t>("i8") == 127);
  BOOST_TEST(querier.get<uint8_t>("u8") == 255);
  BOOST_TEST(querier.get<int16_t>("i16") == 32767);
  BOOST_TEST(querier.get<uint16_t>("u16") == 65535);
  BOOST_TEST(querier.get<int32_t>("i32") == 2147483647);
  BOOST_TEST(querier.get<uint32_t>("u32") == 4294967295);
  BOOST_TEST(querier.get<uint64_t>("i64") == 9223372036854775807);
  BOOST_TEST(querier.get<std::string>("string") == "hello");
  BOOST_TEST(querier.get<std::string>("name") == "lambda");
}
