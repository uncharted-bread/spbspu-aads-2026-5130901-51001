#include "stack.hpp"
#include "queue.hpp"
#include "calc.hpp"
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

BOOST_AUTO_TEST_SUITE(QueueTests)

BOOST_AUTO_TEST_CASE(pushAndFront)
{
  gordejchik::Queue< int > q;
  q.push(10);
  BOOST_CHECK_EQUAL(q.front(), 10);
  BOOST_CHECK_EQUAL(q.back(), 10);
  q.push(20);
  BOOST_CHECK_EQUAL(q.front(), 10);
  BOOST_CHECK_EQUAL(q.back(), 20);
}

BOOST_AUTO_TEST_CASE(fifoOrder)
{
  gordejchik::Queue< int > q;
  q.push(1);
  q.push(2);
  q.push(3);
  BOOST_CHECK_EQUAL(q.front(), 1);
  q.pop();
  BOOST_CHECK_EQUAL(q.front(), 2);
  q.pop();
  BOOST_CHECK_EQUAL(q.front(), 3);
}

BOOST_AUTO_TEST_CASE(emptyAndSize)
{
  gordejchik::Queue< int > q;
  BOOST_CHECK(q.empty());
  BOOST_CHECK_EQUAL(q.size(), 0);
  q.push(42);
  BOOST_CHECK(!q.empty());
  BOOST_CHECK_EQUAL(q.size(), 1);
  q.pop();
  BOOST_CHECK(q.empty());
  BOOST_CHECK_EQUAL(q.size(), 0);
}

BOOST_AUTO_TEST_CASE(throwOnEmpty)
{
  gordejchik::Queue< int > q;
  BOOST_CHECK_THROW(q.front(), std::logic_error);
  BOOST_CHECK_THROW(q.back(), std::logic_error);
  BOOST_CHECK_THROW(q.pop(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(stringValues)
{
  gordejchik::Queue< std::string > q;
  q.push("alpha");
  q.push("beta");
  BOOST_CHECK_EQUAL(q.front(), "alpha");
  BOOST_CHECK_EQUAL(q.back(), "beta");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CalcBinaryTests)

BOOST_AUTO_TEST_CASE(addition)
{
  BOOST_CHECK_EQUAL(gordejchik::calculateExpression("1 + 2"), 3);
}

BOOST_AUTO_TEST_CASE(subtraction)
{
  BOOST_CHECK_EQUAL(gordejchik::calculateExpression("5 - 3"), 2);
}

BOOST_AUTO_TEST_CASE(multiplication)
{
  BOOST_CHECK_EQUAL(gordejchik::calculateExpression("4 * 7"), 28);
}

BOOST_AUTO_TEST_CASE(division)
{
  BOOST_CHECK_EQUAL(gordejchik::calculateExpression("10 / 3"), 3);
}

BOOST_AUTO_TEST_CASE(modulo)
{
  BOOST_CHECK_EQUAL(gordejchik::calculateExpression("10 % 3"), 1);
}

BOOST_AUTO_TEST_CASE(singleNumber)
{
  BOOST_CHECK_EQUAL(gordejchik::calculateExpression("42"), 42);
}

BOOST_AUTO_TEST_CASE(combinedExpression)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("4 * 7 - 3"), 25);
}

BOOST_AUTO_TEST_CASE(parenthesizedExpression)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("( 1 + 2 ) * ( 3 - 4 )"), -3);
}

BOOST_AUTO_TEST_CASE(nestedParentheses)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("( 10 / ( 2 + 3 ) % 4 )"), 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CalcUnaryNotTests)

BOOST_AUTO_TEST_CASE(notZero)
{
  BOOST_CHECK_EQUAL(gordejchik::calculateExpression("! 0"), ~0LL);
}

BOOST_AUTO_TEST_CASE(notPositive)
{
  BOOST_CHECK_EQUAL(gordejchik::calculateExpression("! 5"), ~5LL);
}

BOOST_AUTO_TEST_CASE(doubleNot)
{
  BOOST_CHECK_EQUAL(gordejchik::calculateExpression("! ! 5"), 5);
}

BOOST_AUTO_TEST_CASE(notWithParens)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("! ( 3 + 2 )"), ~5LL);
}

BOOST_AUTO_TEST_CASE(notInBinaryExpr)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("! 5 + 3"), ~5LL + 3);
}

BOOST_AUTO_TEST_CASE(notAfterBinary)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("3 + ! 5"), 3 + ~5LL);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CalcPrecedenceTests)

BOOST_AUTO_TEST_CASE(mulBeforeAdd)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("2 + 3 * 4"), 14);
}

BOOST_AUTO_TEST_CASE(leftAssocSubtraction)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("10 - 3 - 2"), 5);
}

BOOST_AUTO_TEST_CASE(leftAssocDivision)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("100 / 10 / 2"), 5);
}

BOOST_AUTO_TEST_CASE(deepNesting)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("( ( ( 1 + 2 ) ) )"), 3);
}

BOOST_AUTO_TEST_CASE(moduloPrecedence)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("1 + 10 % 3"), 2);
}

BOOST_AUTO_TEST_CASE(complexMixed)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("2 * 3 + 4 * 5"), 26);
}

BOOST_AUTO_TEST_SUITE_END()
