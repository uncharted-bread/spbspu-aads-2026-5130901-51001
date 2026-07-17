#define BOOST_TEST_MODULE F0Tests
#include <string>
#include <utility>
#include <boost/test/included/unit_test.hpp>
#include "hashtable.hpp"

using gordejchik::HashTable;

BOOST_AUTO_TEST_SUITE(HashTableSuite)

BOOST_AUTO_TEST_CASE(emptyTable)
{
  HashTable< std::string, int > ht;
  BOOST_TEST(ht.empty());
  BOOST_TEST(ht.size() == 0u);
  BOOST_TEST(!ht.contains("any"));
}

BOOST_AUTO_TEST_CASE(insertAndFind)
{
  HashTable< std::string, int > ht;
  ht.insert("alpha", 1);
  ht.insert("beta", 2);
  ht.insert("gamma", 3);

  BOOST_TEST(ht.size() == 3u);
  BOOST_TEST(ht.at("alpha") == 1);
  BOOST_TEST(ht.at("beta") == 2);
  BOOST_TEST(ht.at("gamma") == 3);
  BOOST_TEST(!ht.contains("delta"));
}

BOOST_AUTO_TEST_CASE(overwriteValue)
{
  HashTable< std::string, int > ht;
  ht.insert("key", 10);
  ht.insert("key", 20);
  BOOST_TEST(ht.at("key") == 20);
  BOOST_TEST(ht.size() == 1u);
}

BOOST_AUTO_TEST_CASE(atNonExisting)
{
  const HashTable< std::string, int > ht;
  BOOST_CHECK_THROW(ht.at("x"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(eraseExisting)
{
  HashTable< std::string, int > ht;
  ht.insert("a", 1);
  ht.insert("b", 2);
  ht.insert("c", 3);

  ht.erase("b");
  BOOST_TEST(ht.size() == 2u);
  BOOST_TEST(!ht.contains("b"));
  BOOST_TEST(ht.contains("a"));
  BOOST_TEST(ht.contains("c"));
}

BOOST_AUTO_TEST_CASE(eraseNonExisting)
{
  HashTable< std::string, int > ht;
  BOOST_CHECK_THROW(ht.erase("z"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(rehashPreservesData)
{
  HashTable< int, int > ht(4);
  ht.insert(1, 10);
  ht.insert(2, 20);
  ht.insert(3, 30);
  ht.insert(4, 40);

  BOOST_TEST(ht.capacity() > 4u);
  BOOST_TEST(ht.size() == 4u);
  BOOST_TEST(ht.at(1) == 10);
  BOOST_TEST(ht.at(4) == 40);
}

BOOST_AUTO_TEST_CASE(manyInsertionsAndDeletions)
{
  HashTable< int, int > ht;
  const int count = 200;
  for (int i = 0; i < count; ++i) {
    ht.insert(i, i * 10);
  }
  BOOST_TEST(ht.size() == 200u);

  for (int i = 0; i < count; i += 2) {
    ht.erase(i);
  }
  BOOST_TEST(ht.size() == 100u);

  for (int i = 1; i < count; i += 2) {
    BOOST_TEST(ht.contains(i));
    BOOST_TEST(ht.at(i) == i * 10);
  }
}

BOOST_AUTO_TEST_CASE(copyConstructor)
{
  HashTable< std::string, int > ht;
  ht.insert("x", 100);
  ht.insert("y", 200);

  HashTable< std::string, int > copy(ht);
  BOOST_TEST(copy.size() == 2u);
  BOOST_TEST(copy.at("x") == 100);

  copy.insert("x", 999);
  BOOST_TEST(ht.at("x") == 100);
}

BOOST_AUTO_TEST_CASE(moveConstructor)
{
  HashTable< std::string, int > ht;
  ht.insert("a", 1);

  HashTable< std::string, int > moved(std::move(ht));
  BOOST_TEST(moved.size() == 1u);
  BOOST_TEST(moved.at("a") == 1);
  BOOST_TEST(ht.size() == 0u);
}

BOOST_AUTO_TEST_CASE(findExisting)
{
  HashTable< std::string, int > ht;
  ht.insert("a", 1);
  ht.insert("b", 2);

  using Iter = HashTable< std::string, int >::Iterator;
  Iter it = ht.find("b");
  BOOST_TEST((it != ht.end()));
  BOOST_TEST(it->second == 2);

  it->second = 22;
  BOOST_TEST(ht.at("b") == 22);
}

BOOST_AUTO_TEST_CASE(findMissing)
{
  HashTable< std::string, int > ht;
  ht.insert("a", 1);
  BOOST_TEST((ht.find("z") == ht.end()));
}

BOOST_AUTO_TEST_CASE(constFind)
{
  HashTable< std::string, int > ht;
  ht.insert("x", 10);

  const HashTable< std::string, int >& ref = ht;
  using CIter = HashTable< std::string, int >::ConstIterator;
  CIter it = ref.find("x");
  BOOST_TEST((it != ref.cend()));
  BOOST_TEST(it->second == 10);
  BOOST_TEST((ref.find("nope") == ref.cend()));
}

BOOST_AUTO_TEST_CASE(emptyTableIteration)
{
  HashTable< std::string, int > ht;
  BOOST_TEST((ht.begin() == ht.end()));
  BOOST_TEST((ht.cbegin() == ht.cend()));
}

BOOST_AUTO_TEST_CASE(iteratorTraversal)
{
  HashTable< std::string, int > ht;
  ht.insert("a", 1);
  ht.insert("b", 2);
  ht.insert("c", 3);

  using Iter = HashTable< std::string, int >::Iterator;
  int sum = 0;
  size_t visited = 0;
  for (Iter it = ht.begin(); it != ht.end(); ++it) {
    sum += it->second;
    ++visited;
  }
  BOOST_TEST(sum == 6);
  BOOST_TEST(visited == 3u);
}

BOOST_AUTO_TEST_CASE(iteratorWriteAccess)
{
  HashTable< std::string, int > ht;
  ht.insert("key", 1);

  using Iter = HashTable< std::string, int >::Iterator;
  Iter it = ht.begin();
  (*it).second = 42;
  BOOST_TEST(ht.at("key") == 42);
}

BOOST_AUTO_TEST_CASE(postIncrementReturnsOldPosition)
{
  HashTable< std::string, int > ht;
  ht.insert("only", 7);

  using Iter = HashTable< std::string, int >::Iterator;
  Iter it = ht.begin();
  Iter old = it++;
  BOOST_TEST(old->second == 7);
  BOOST_TEST((it == ht.end()));
}

BOOST_AUTO_TEST_CASE(constIteration)
{
  HashTable< std::string, int > ht;
  ht.insert("x", 10);
  ht.insert("y", 20);

  const HashTable< std::string, int >& ref = ht;
  using CIter = HashTable< std::string, int >::ConstIterator;
  int sum = 0;
  for (CIter it = ref.begin(); it != ref.end(); ++it) {
    sum += it->second;
  }
  BOOST_TEST(sum == 30);
}

BOOST_AUTO_TEST_CASE(iteratorConvertsToConstIterator)
{
  HashTable< std::string, int > ht;
  ht.insert("z", 5);

  using CIter = HashTable< std::string, int >::ConstIterator;
  CIter cit = ht.begin();
  BOOST_TEST(cit->second == 5);
}

BOOST_AUTO_TEST_SUITE_END()
