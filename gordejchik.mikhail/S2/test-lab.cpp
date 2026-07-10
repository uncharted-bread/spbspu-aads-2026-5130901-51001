#define BOOST_TEST_MODULE S2
#include <limits>
#include <string>
#include <utility>
#include <boost/test/included/unit_test.hpp>
#include "calc.hpp"
#include "list.hpp"
#include "queue.hpp"
#include "stack.hpp"

class Tracked
{
public:
  static size_t copyCount;
  static size_t moveCount;

  Tracked(int first, int second);
  Tracked(const Tracked& other);
  Tracked(Tracked&& other) noexcept;

  int sum() const;

private:
  int first_;
  int second_;
};

size_t Tracked::copyCount = 0;
size_t Tracked::moveCount = 0;

Tracked::Tracked(int first, int second):
  first_(first),
  second_(second)
{}

Tracked::Tracked(const Tracked& other):
  first_(other.first_),
  second_(other.second_)
{
  ++Tracked::copyCount;
}

Tracked::Tracked(Tracked&& other) noexcept:
  first_(other.first_),
  second_(other.second_)
{
  ++Tracked::moveCount;
}

int Tracked::sum() const
{
  return first_ + second_;
}

static void resetTrackedCounters()
{
  Tracked::copyCount = 0;
  Tracked::moveCount = 0;
}

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

BOOST_AUTO_TEST_SUITE(ListEmplaceTests)

BOOST_AUTO_TEST_CASE(emplaceBackConstructsInPlace)
{
  gordejchik::List< Tracked > list;
  resetTrackedCounters();
  list.emplaceBack(40, 2);
  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list.back().sum(), 42);
  BOOST_CHECK_EQUAL(Tracked::copyCount, 0);
  BOOST_CHECK_EQUAL(Tracked::moveCount, 0);
}

BOOST_AUTO_TEST_CASE(emplaceFrontConstructsInPlace)
{
  gordejchik::List< Tracked > list;
  resetTrackedCounters();
  list.emplaceFront(1, 2);
  list.emplaceFront(3, 4);
  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(list.front().sum(), 7);
  BOOST_CHECK_EQUAL(list.back().sum(), 3);
  BOOST_CHECK_EQUAL(Tracked::copyCount, 0);
  BOOST_CHECK_EQUAL(Tracked::moveCount, 0);
}

BOOST_AUTO_TEST_CASE(emplaceAfterInsertsInMiddle)
{
  gordejchik::List< int > list;
  list.pushBack(1);
  list.pushBack(3);
  gordejchik::List< int >::iterator result = list.emplaceAfter(list.begin(), 2);
  BOOST_CHECK_EQUAL(*result, 2);
  gordejchik::List< int >::const_iterator it = list.cbegin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
  BOOST_CHECK_EQUAL(list.size(), 3);
}

BOOST_AUTO_TEST_CASE(emplaceAfterTailAppends)
{
  gordejchik::List< int > list;
  list.pushBack(1);
  gordejchik::List< int >::iterator result = list.emplaceAfter(list.begin(), 2);
  BOOST_CHECK_EQUAL(*result, 2);
  BOOST_CHECK_EQUAL(list.back(), 2);
  BOOST_CHECK_EQUAL(list.size(), 2);
}

BOOST_AUTO_TEST_CASE(emplaceAfterConstructsInPlace)
{
  gordejchik::List< Tracked > list;
  list.emplaceBack(1, 1);
  resetTrackedCounters();
  list.emplaceAfter(list.cbegin(), 2, 2);
  BOOST_CHECK_EQUAL(list.back().sum(), 4);
  BOOST_CHECK_EQUAL(Tracked::copyCount, 0);
  BOOST_CHECK_EQUAL(Tracked::moveCount, 0);
}

BOOST_AUTO_TEST_CASE(emplaceMultiArgumentConstructor)
{
  gordejchik::List< std::string > list;
  list.emplaceBack(3, 'a');
  BOOST_CHECK_EQUAL(list.back(), "aaa");
}

BOOST_AUTO_TEST_CASE(emplaceForwardsLvalueAsCopy)
{
  gordejchik::List< Tracked > list;
  Tracked original(20, 22);
  resetTrackedCounters();
  list.emplaceBack(original);
  BOOST_CHECK_EQUAL(list.back().sum(), 42);
  BOOST_CHECK_EQUAL(Tracked::copyCount, 1);
  BOOST_CHECK_EQUAL(Tracked::moveCount, 0);
}

BOOST_AUTO_TEST_CASE(emplaceForwardsRvalueAsMove)
{
  gordejchik::List< Tracked > list;
  Tracked original(20, 22);
  resetTrackedCounters();
  list.emplaceBack(std::move(original));
  BOOST_CHECK_EQUAL(list.back().sum(), 42);
  BOOST_CHECK_EQUAL(Tracked::copyCount, 0);
  BOOST_CHECK_EQUAL(Tracked::moveCount, 1);
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

BOOST_AUTO_TEST_SUITE(CalcOverflowTests)

BOOST_AUTO_TEST_CASE(additionOverflow)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("9223372036854775807 + 1"),
      std::overflow_error);
}

BOOST_AUTO_TEST_CASE(additionAtBoundary)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("9223372036854775806 + 1"),
      std::numeric_limits< long long >::max());
}

BOOST_AUTO_TEST_CASE(subtractionOverflow)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("! 9223372036854775807 - 1"),
      std::overflow_error);
}

BOOST_AUTO_TEST_CASE(multiplicationOverflow)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("4611686018427387904 * 2"),
      std::overflow_error);
}

BOOST_AUTO_TEST_CASE(negativeMultiplicationOverflow)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("! 4611686018427387904 * 2"),
      std::overflow_error);
}

BOOST_AUTO_TEST_CASE(divisionOverflow)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("! 9223372036854775807 / ! 0"),
      std::overflow_error);
}

BOOST_AUTO_TEST_CASE(moduloByMinusOne)
{
  BOOST_CHECK_EQUAL(
      gordejchik::calculateExpression("! 9223372036854775807 % ! 0"), 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CalcErrorTests)

BOOST_AUTO_TEST_CASE(unclosedParen)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("( 1 + 2"),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(extraClosingParen)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("1 + 2 )"),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(unknownToken)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("1 + abc"),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(divisionByZero)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("1 / 0"),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(moduloByZero)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("7 % 0"),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(missingOperand)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("1 +"),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(emptyExpression)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression(""),
      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(notWithoutOperand)
{
  BOOST_CHECK_THROW(
      gordejchik::calculateExpression("!"),
      std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
