#define BOOST_TEST_MODULE ListTest
#include <functional>
#include <string>
#include <utility>
#include <boost/test/included/unit_test.hpp>
#include "list.hpp"

using gordejchik::List;
using gordejchik::LIter;
using gordejchik::LCIter;

static std::string toStr(const List< int >& lst)
{
  std::string result;
  for (List< int >::const_iterator it = lst.cbegin(); it != lst.cend(); ++it) {
    if (!result.empty()) {
      result += ' ';
    }
    result += std::to_string(*it);
  }
  return result;
}

BOOST_AUTO_TEST_SUITE(ListSuite)

BOOST_AUTO_TEST_CASE(emptyList)
{
  const List< int > lst;
  BOOST_TEST(lst.empty());
  BOOST_TEST(lst.size() == 0u);
  BOOST_CHECK(lst.begin() == lst.end());
  BOOST_CHECK(lst.cbegin() == lst.cend());
}

BOOST_AUTO_TEST_CASE(pushBackAndIterate)
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.pushBack(3);

  BOOST_TEST(!lst.empty());
  BOOST_TEST(lst.size() == 3u);
  BOOST_TEST(lst.front() == 1);
  BOOST_TEST(lst.back() == 3);

  List< int >::iterator it = lst.begin();
  BOOST_TEST(*it == 1);
  ++it;
  BOOST_TEST(*it == 2);
  ++it;
  BOOST_TEST(*it == 3);
  ++it;
  BOOST_CHECK(it == lst.end());
}

BOOST_AUTO_TEST_CASE(pushFrontAndIterate)
{
  List< int > lst;
  lst.pushFront(3);
  lst.pushFront(2);
  lst.pushFront(1);

  BOOST_TEST(lst.size() == 3u);
  BOOST_TEST(lst.front() == 1);
  BOOST_TEST(lst.back() == 3);

  List< int >::iterator it = lst.begin();
  BOOST_TEST(*it == 1);
  ++it;
  BOOST_TEST(*it == 2);
  ++it;
  BOOST_TEST(*it == 3);
}

BOOST_AUTO_TEST_CASE(popFront)
{
  List< int > lst;
  lst.pushBack(10);
  lst.pushBack(20);
  lst.pushBack(30);

  lst.popFront();
  BOOST_TEST(lst.size() == 2u);
  BOOST_TEST(lst.front() == 20);

  lst.popFront();
  lst.popFront();
  BOOST_TEST(lst.empty());
  BOOST_CHECK(lst.begin() == lst.end());
}

BOOST_AUTO_TEST_CASE(popBack)
{
  List< int > lst;
  lst.pushBack(10);
  lst.pushBack(20);
  lst.pushBack(30);

  lst.popBack();
  BOOST_TEST(lst.size() == 2u);
  BOOST_TEST(lst.back() == 20);

  lst.popBack();
  lst.popBack();
  BOOST_TEST(lst.empty());
}

BOOST_AUTO_TEST_CASE(bidirectionalIteration)
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.pushBack(3);

  List< int >::iterator it = lst.begin();
  ++it;
  ++it;
  BOOST_TEST(*it == 3);
  --it;
  BOOST_TEST(*it == 2);
  --it;
  BOOST_TEST(*it == 1);
  BOOST_CHECK(it == lst.begin());
}

BOOST_AUTO_TEST_CASE(reverseFromEnd)
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.pushBack(3);

  List< int >::iterator it = lst.end();
  --it;
  BOOST_TEST(*it == 3);
  --it;
  BOOST_TEST(*it == 2);
  --it;
  BOOST_TEST(*it == 1);
  BOOST_CHECK(it == lst.begin());
}

BOOST_AUTO_TEST_CASE(postfixOperators)
{
  List< int > lst;
  lst.pushBack(7);
  lst.pushBack(8);

  List< int >::iterator it = lst.begin();
  List< int >::iterator prev = it++;
  BOOST_TEST(*prev == 7);
  BOOST_TEST(*it == 8);

  List< int >::iterator prev2 = it--;
  BOOST_TEST(*prev2 == 8);
  BOOST_TEST(*it == 7);
}

BOOST_AUTO_TEST_CASE(constIterator)
{
  List< int > lst;
  lst.pushBack(5);
  lst.pushBack(6);

  const List< int >& cref = lst;
  List< int >::const_iterator it = cref.cbegin();
  BOOST_TEST(*it == 5);
  ++it;
  BOOST_TEST(*it == 6);
  ++it;
  BOOST_CHECK(it == cref.cend());
}

BOOST_AUTO_TEST_CASE(iteratorToConstIterator)
{
  List< int > lst;
  lst.pushBack(42);

  LCIter< int > cit = lst.begin();
  BOOST_TEST(*cit == 42);
}

BOOST_AUTO_TEST_CASE(copyConstructor)
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(2);
  a.pushBack(3);

  List< int > b(a);
  BOOST_TEST(b.size() == 3u);
  BOOST_TEST(b.front() == 1);
  BOOST_TEST(b.back() == 3);

  b.popFront();
  BOOST_TEST(b.size() == 2u);
  BOOST_TEST(a.size() == 3u);
}

BOOST_AUTO_TEST_CASE(copyAssignment)
{
  List< int > a;
  a.pushBack(10);
  a.pushBack(20);

  List< int > b;
  b.pushBack(99);
  b = a;

  BOOST_TEST(b.size() == 2u);
  BOOST_TEST(b.front() == 10);
  BOOST_TEST(b.back() == 20);

  b = b;
  BOOST_TEST(b.size() == 2u);
}

BOOST_AUTO_TEST_CASE(moveConstructor)
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(2);

  List< int > b(std::move(a));
  BOOST_TEST(b.size() == 2u);
  BOOST_TEST(b.front() == 1);
  BOOST_TEST(b.back() == 2);
  BOOST_TEST(a.empty());
}

BOOST_AUTO_TEST_CASE(moveAssignment)
{
  List< int > a;
  a.pushBack(3);
  a.pushBack(4);

  List< int > b;
  b = std::move(a);
  BOOST_TEST(b.size() == 2u);
  BOOST_TEST(b.front() == 3);
  BOOST_TEST(a.empty());
}

BOOST_AUTO_TEST_CASE(insertMiddle)
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(3);

  List< int >::const_iterator it = lst.cbegin();
  ++it;
  lst.insert(it, 2);

  BOOST_TEST(lst.size() == 3u);
  List< int >::iterator cur = lst.begin();
  BOOST_TEST(*cur == 1);
  ++cur;
  BOOST_TEST(*cur == 2);
  ++cur;
  BOOST_TEST(*cur == 3);
}

BOOST_AUTO_TEST_CASE(insertAtBegin)
{
  List< int > lst;
  lst.pushBack(2);
  lst.insert(lst.cbegin(), 1);
  BOOST_TEST(lst.front() == 1);
  BOOST_TEST(lst.size() == 2u);
}

BOOST_AUTO_TEST_CASE(insertAtEnd)
{
  List< int > lst;
  lst.pushBack(1);
  lst.insert(lst.cend(), 2);
  BOOST_TEST(lst.back() == 2);
  BOOST_TEST(lst.size() == 2u);
}

BOOST_AUTO_TEST_CASE(erase)
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.pushBack(3);

  List< int >::iterator it = lst.begin();
  ++it;
  it = lst.erase(it);

  BOOST_TEST(lst.size() == 2u);
  BOOST_TEST(*it == 3);
  BOOST_TEST(lst.front() == 1);
  BOOST_TEST(lst.back() == 3);
}

BOOST_AUTO_TEST_CASE(clear)
{
  List< int > lst;
  lst.pushBack(1);
  lst.pushBack(2);
  lst.clear();

  BOOST_TEST(lst.empty());
  BOOST_TEST(lst.size() == 0u);
  BOOST_CHECK(lst.begin() == lst.end());

  lst.pushBack(42);
  BOOST_TEST(lst.size() == 1u);
  BOOST_TEST(lst.front() == 42);
}

BOOST_AUTO_TEST_CASE(singleElement)
{
  List< int > lst;
  lst.pushBack(99);
  BOOST_TEST(lst.size() == 1u);
  BOOST_TEST(lst.front() == 99);
  BOOST_TEST(lst.back() == 99);

  lst.popFront();
  BOOST_TEST(lst.empty());

  lst.pushBack(99);
  lst.popBack();
  BOOST_TEST(lst.empty());
}

BOOST_AUTO_TEST_CASE(swap)
{
  List< int > a;
  List< int > b;
  a.pushBack(1);
  a.pushBack(2);
  b.pushBack(10);

  a.swap(b);

  BOOST_TEST(a.size() == 1u);
  BOOST_TEST(a.front() == 10);
  BOOST_TEST(b.size() == 2u);
  BOOST_TEST(b.front() == 1);
}

BOOST_AUTO_TEST_CASE(swapWithEmpty)
{
  List< int > a;
  List< int > b;
  a.pushBack(1);
  a.pushBack(2);

  a.swap(b);

  BOOST_TEST(a.empty());
  BOOST_TEST(b.size() == 2u);
  BOOST_TEST(b.front() == 1);
  BOOST_TEST(b.back() == 2);
}

BOOST_AUTO_TEST_CASE(spliceWholeList)
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(4);
  List< int > b;
  b.pushBack(2);
  b.pushBack(3);

  List< int >::const_iterator pos = a.cbegin();
  ++pos;
  a.splice(pos, b);

  BOOST_TEST(toStr(a) == "1 2 3 4");
  BOOST_TEST(a.size() == 4u);
  BOOST_TEST(b.empty());
}

BOOST_AUTO_TEST_CASE(spliceEmptySource)
{
  List< int > a;
  a.pushBack(1);
  List< int > b;

  a.splice(a.cend(), b);

  BOOST_TEST(toStr(a) == "1");
  BOOST_TEST(a.size() == 1u);
  BOOST_TEST(b.empty());
}

BOOST_AUTO_TEST_CASE(spliceSingleElement)
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(3);
  List< int > b;
  b.pushBack(2);
  b.pushBack(9);

  List< int >::const_iterator pos = a.cbegin();
  ++pos;
  a.splice(pos, b, b.cbegin());

  BOOST_TEST(toStr(a) == "1 2 3");
  BOOST_TEST(toStr(b) == "9");
  BOOST_TEST(a.size() == 3u);
  BOOST_TEST(b.size() == 1u);
}

BOOST_AUTO_TEST_CASE(spliceSingleNoEffect)
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(2);

  a.splice(a.cbegin(), a, a.cbegin());
  BOOST_TEST(toStr(a) == "1 2");

  List< int >::const_iterator second = a.cbegin();
  ++second;
  a.splice(second, a, a.cbegin());
  BOOST_TEST(toStr(a) == "1 2");
  BOOST_TEST(a.size() == 2u);
}

BOOST_AUTO_TEST_CASE(spliceRange)
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(5);
  List< int > b;
  b.pushBack(2);
  b.pushBack(3);
  b.pushBack(4);
  b.pushBack(9);

  List< int >::const_iterator pos = a.cbegin();
  ++pos;
  List< int >::const_iterator last = b.cbegin();
  ++last;
  ++last;
  ++last;
  a.splice(pos, b, b.cbegin(), last);

  BOOST_TEST(toStr(a) == "1 2 3 4 5");
  BOOST_TEST(toStr(b) == "9");
  BOOST_TEST(a.size() == 5u);
  BOOST_TEST(b.size() == 1u);
}

BOOST_AUTO_TEST_CASE(spliceRangeInsideSameList)
{
  List< int > a;
  a.pushBack(3);
  a.pushBack(4);
  a.pushBack(1);
  a.pushBack(2);

  List< int >::const_iterator first = a.cbegin();
  ++first;
  ++first;
  a.splice(a.cbegin(), a, first, a.cend());

  BOOST_TEST(toStr(a) == "1 2 3 4");
  BOOST_TEST(a.size() == 4u);
}

BOOST_AUTO_TEST_CASE(mergeSortedLists)
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(3);
  a.pushBack(5);
  List< int > b;
  b.pushBack(2);
  b.pushBack(4);
  b.pushBack(6);

  a.merge(b);

  BOOST_TEST(toStr(a) == "1 2 3 4 5 6");
  BOOST_TEST(a.size() == 6u);
  BOOST_TEST(b.empty());
}

BOOST_AUTO_TEST_CASE(mergeWithEmpty)
{
  List< int > a;
  a.pushBack(1);
  List< int > b;

  a.merge(b);
  BOOST_TEST(toStr(a) == "1");

  b.merge(a);
  BOOST_TEST(toStr(b) == "1");
  BOOST_TEST(a.empty());
}

BOOST_AUTO_TEST_CASE(mergeWithSelf)
{
  List< int > a;
  a.pushBack(1);
  a.pushBack(2);

  a.merge(a);

  BOOST_TEST(toStr(a) == "1 2");
  BOOST_TEST(a.size() == 2u);
}

BOOST_AUTO_TEST_CASE(mergeWithComparator)
{
  List< int > a;
  a.pushBack(5);
  a.pushBack(3);
  a.pushBack(1);
  List< int > b;
  b.pushBack(6);
  b.pushBack(4);
  b.pushBack(2);

  a.merge(b, std::greater< int >());

  BOOST_TEST(toStr(a) == "6 5 4 3 2 1");
  BOOST_TEST(b.empty());
}

BOOST_AUTO_TEST_CASE(sortUnsorted)
{
  List< int > lst;
  lst.pushBack(3);
  lst.pushBack(1);
  lst.pushBack(4);
  lst.pushBack(1);
  lst.pushBack(5);
  lst.pushBack(9);
  lst.pushBack(2);
  lst.pushBack(6);

  lst.sort();

  BOOST_TEST(toStr(lst) == "1 1 2 3 4 5 6 9");
  BOOST_TEST(lst.size() == 8u);
}

BOOST_AUTO_TEST_CASE(sortReversed)
{
  List< int > lst;
  for (int i = 5; i > 0; --i) {
    lst.pushBack(i);
  }

  lst.sort();

  BOOST_TEST(toStr(lst) == "1 2 3 4 5");
}

BOOST_AUTO_TEST_CASE(sortSmallLists)
{
  List< int > lst;
  lst.sort();
  BOOST_TEST(lst.empty());

  lst.pushBack(7);
  lst.sort();
  BOOST_TEST(toStr(lst) == "7");

  lst.pushBack(6);
  lst.sort();
  BOOST_TEST(toStr(lst) == "6 7");
}

BOOST_AUTO_TEST_CASE(sortWithComparator)
{
  List< int > lst;
  lst.pushBack(2);
  lst.pushBack(5);
  lst.pushBack(1);

  lst.sort(std::greater< int >());

  BOOST_TEST(toStr(lst) == "5 2 1");
}

BOOST_AUTO_TEST_CASE(sortStrings)
{
  List< std::string > lst;
  lst.pushBack("pear");
  lst.pushBack("apple");
  lst.pushBack("orange");

  lst.sort();

  BOOST_TEST(lst.front() == "apple");
  BOOST_TEST(lst.back() == "pear");
  BOOST_TEST(lst.size() == 3u);
}

BOOST_AUTO_TEST_CASE(stringList)
{
  List< std::string > lst;
  lst.pushBack("hello");
  lst.pushBack("world");
  BOOST_TEST(lst.front() == "hello");
  BOOST_TEST(lst.back() == "world");
  BOOST_TEST(lst.size() == 2u);
}

BOOST_AUTO_TEST_SUITE_END()
