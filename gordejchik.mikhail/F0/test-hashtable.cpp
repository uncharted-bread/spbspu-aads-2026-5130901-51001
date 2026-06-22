#define BOOST_TEST_MODULE HashTableTest
#include <boost/test/included/unit_test.hpp>
#include <string>
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

BOOST_AUTO_TEST_SUITE_END()
