#include "stack.hpp"
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(StackTests)

BOOST_AUTO_TEST_CASE(pushAndTop)
{
  gordejchik::Stack< int > s;
  s.push(10);
  BOOST_CHECK_EQUAL(s.top(), 10);
  s.push(20);
  BOOST_CHECK_EQUAL(s.top(), 20);
}

BOOST_AUTO_TEST_CASE(lifoOrder)
{
  gordejchik::Stack< int > s;
  s.push(1);
  s.push(2);
  s.push(3);
  BOOST_CHECK_EQUAL(s.top(), 3);
  s.pop();
  BOOST_CHECK_EQUAL(s.top(), 2);
  s.pop();
  BOOST_CHECK_EQUAL(s.top(), 1);
}

BOOST_AUTO_TEST_CASE(emptyAndSize)
{
  gordejchik::Stack< int > s;
  BOOST_CHECK(s.empty());
  BOOST_CHECK_EQUAL(s.size(), 0);
  s.push(42);
  BOOST_CHECK(!s.empty());
  BOOST_CHECK_EQUAL(s.size(), 1);
  s.pop();
  BOOST_CHECK(s.empty());
  BOOST_CHECK_EQUAL(s.size(), 0);
}

BOOST_AUTO_TEST_CASE(throwOnEmpty)
{
  gordejchik::Stack< int > s;
  BOOST_CHECK_THROW(s.top(), std::logic_error);
  BOOST_CHECK_THROW(s.pop(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(stringValues)
{
  gordejchik::Stack< std::string > s;
  s.push("hello");
  s.push("world");
  BOOST_CHECK_EQUAL(s.top(), "world");
  s.pop();
  BOOST_CHECK_EQUAL(s.top(), "hello");
}

BOOST_AUTO_TEST_SUITE_END()
