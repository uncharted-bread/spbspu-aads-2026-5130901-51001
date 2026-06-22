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

BOOST_AUTO_TEST_SUITE_END()
