#define BOOST_TEST_MODULE HashTableTests
#include <stdexcept>
#include <utility>
#include <boost/test/included/unit_test.hpp>
#include "hash-table.hpp"
#include "sha1.hpp"

using Table = gordejchik::HashTable< std::string, int,
    gordejchik::Sha1Hash< std::string >, gordejchik::StringEqual >;

BOOST_AUTO_TEST_CASE(insert_and_contains)
{
  Table t(4, 3);
  t.insert("alpha", 1);
  t.insert("beta", 2);
  t.insert("gamma", 3);
  BOOST_TEST(t.contains("alpha"));
  BOOST_TEST(t.contains("beta"));
  BOOST_TEST(t.contains("gamma"));
  BOOST_TEST(!t.contains("delta"));
  BOOST_TEST(t.size() == 3);
}

BOOST_AUTO_TEST_CASE(at_and_bracket)
{
  Table t(4, 3);
  t.insert("x", 10);
  BOOST_TEST(t.at("x") == 10);
  t["y"] = 20;
  BOOST_TEST(t.at("y") == 20);
  t.insert("x", 99);
  BOOST_TEST(t.at("x") == 99);
  BOOST_CHECK_THROW(t.at("z"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(erase_element)
{
  Table t(4, 3);
  t.insert("a", 1);
  t.insert("b", 2);
  t.erase("a");
  BOOST_TEST(!t.contains("a"));
  BOOST_TEST(t.contains("b"));
  BOOST_TEST(t.size() == 1);
  BOOST_CHECK_THROW(t.erase("a"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(overflow_exception)
{
  Table t(2, 2);
  t.insert("a", 1);
  t.insert("b", 2);
  t.insert("c", 3);
  t.insert("d", 4);
  t.insert("e", 5);
  t.insert("f", 6);
  BOOST_CHECK_THROW(t.insert("g", 7), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(rehash_preserves_data)
{
  Table t(2, 2);
  t.insert("one", 1);
  t.insert("two", 2);
  t.insert("three", 3);
  t.rehash(8);
  BOOST_TEST(t.bucketCount() == 8);
  BOOST_TEST(t.at("one") == 1);
  BOOST_TEST(t.at("two") == 2);
  BOOST_TEST(t.at("three") == 3);
  BOOST_TEST(t.size() == 3);
}

BOOST_AUTO_TEST_CASE(copy_and_move)
{
  Table t1(4, 3);
  t1.insert("k1", 10);
  t1.insert("k2", 20);

  Table t2(t1);
  BOOST_TEST(t2.at("k1") == 10);
  BOOST_TEST(t2.at("k2") == 20);
  t2.insert("k1", 99);
  BOOST_TEST(t1.at("k1") == 10);

  Table t3(std::move(t1));
  BOOST_TEST(t3.at("k2") == 20);
  BOOST_TEST(t3.size() == 2);
}

BOOST_AUTO_TEST_CASE(iteration)
{
  Table t(4, 3);
  t.insert("a", 1);
  t.insert("b", 2);
  t.insert("c", 3);
  int sum = 0;
  size_t count = 0;
  for (Table::Iterator it = t.begin(); it != t.end(); ++it) {
    sum += it->second;
    ++count;
  }
  BOOST_TEST(sum == 6);
  BOOST_TEST(count == 3);
}

BOOST_AUTO_TEST_CASE(clear_table)
{
  Table t(4, 3);
  t.insert("a", 1);
  t.insert("b", 2);
  t.clear();
  BOOST_TEST(t.empty());
  BOOST_TEST(t.size() == 0);
  BOOST_TEST(!t.contains("a"));
}
