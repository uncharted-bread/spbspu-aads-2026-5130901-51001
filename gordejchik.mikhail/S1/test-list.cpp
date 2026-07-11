#define BOOST_TEST_MODULE ListTest
#include <boost/test/included/unit_test.hpp>
#include <string>
#include "list.hpp"

using gordejchik::List;
using gordejchik::LIter;
using gordejchik::LCIter;

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

  List< int > b(static_cast< List< int >&& >(a));
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
  b = static_cast< List< int >&& >(a);
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
