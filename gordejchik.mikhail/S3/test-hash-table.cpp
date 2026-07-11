#define BOOST_TEST_MODULE HashTableTests
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <boost/test/included/unit_test.hpp>
#include "hash-table.hpp"
#include "sha1.hpp"

using Table = gordejchik::HashTable< std::string, int,
    gordejchik::Sha1Hash< std::string >, gordejchik::StringEqual >;

struct ZeroHash {
  size_t operator()(const std::string& key) const;
};

size_t ZeroHash::operator()(const std::string&) const
{
  return 0;
}

using CollidingTable = gordejchik::HashTable< std::string, int,
    ZeroHash, gordejchik::StringEqual >;

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

BOOST_AUTO_TEST_CASE(spare_bucket_lookup_and_erase)
{
  CollidingTable t(4, 2);
  t.insert("a", 1);
  t.insert("b", 2);
  t.insert("c", 3);
  BOOST_TEST(t.at("c") == 3);
  BOOST_TEST(t.size() == 3);
  t.erase("c");
  BOOST_TEST(!t.contains("c"));
  t.insert("d", 4);
  BOOST_TEST(t.at("d") == 4);
  BOOST_TEST(t.at("a") == 1);
  BOOST_TEST(t.at("b") == 2);
}

BOOST_AUTO_TEST_CASE(rehash_keeps_colliding_elements)
{
  CollidingTable t(2, 2);
  t.insert("a", 1);
  t.insert("b", 2);
  t.insert("c", 3);
  t.rehash(4);
  BOOST_TEST(t.size() == 3);
  BOOST_TEST(t.at("a") == 1);
  BOOST_TEST(t.at("b") == 2);
  BOOST_TEST(t.at("c") == 3);
}

BOOST_AUTO_TEST_CASE(copy_assignment)
{
  Table t1(4, 3);
  t1.insert("k", 1);
  Table t2(2, 2);
  t2.insert("other", 5);
  t2 = t1;
  BOOST_TEST(t2.at("k") == 1);
  BOOST_TEST(!t2.contains("other"));
  t2.insert("k", 7);
  BOOST_TEST(t1.at("k") == 1);
}

BOOST_AUTO_TEST_CASE(move_assignment)
{
  Table t1(4, 3);
  t1.insert("k", 1);
  Table t2(2, 2);
  t2.insert("other", 5);
  t2 = std::move(t1);
  BOOST_TEST(t2.at("k") == 1);
  BOOST_TEST(t2.size() == 1);
  BOOST_TEST(!t2.contains("other"));
}

BOOST_AUTO_TEST_CASE(self_assignment)
{
  Table t(4, 3);
  t.insert("k", 1);
  Table& ref = t;
  t = ref;
  BOOST_TEST(t.at("k") == 1);
  BOOST_TEST(t.size() == 1);
}

BOOST_AUTO_TEST_CASE(empty_table_iteration)
{
  Table t(4, 3);
  BOOST_TEST((t.begin() == t.end()));
  const Table& ct = t;
  BOOST_TEST((ct.cbegin() == ct.cend()));
}

BOOST_AUTO_TEST_CASE(const_iteration)
{
  Table t(4, 3);
  t.insert("a", 1);
  t.insert("b", 2);
  const Table& ct = t;
  int sum = 0;
  for (Table::ConstIterator it = ct.begin(); it != ct.end(); ++it) {
    sum += it->second;
  }
  BOOST_TEST(sum == 3);
  Table::ConstIterator converted = t.begin();
  BOOST_TEST((converted != ct.end()));
}

BOOST_AUTO_TEST_CASE(insert_after_clear)
{
  Table t(4, 3);
  t.insert("a", 1);
  t.clear();
  t.insert("b", 2);
  BOOST_TEST(t.size() == 1);
  BOOST_TEST(t.at("b") == 2);
  BOOST_TEST(!t.contains("a"));
}

BOOST_AUTO_TEST_CASE(zero_size_constructor_throws)
{
  BOOST_CHECK_THROW(Table(0, 3), std::invalid_argument);
  BOOST_CHECK_THROW(Table(3, 0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(bracket_operator_updates)
{
  Table t(4, 3);
  t["k"] = 5;
  t["k"] = 6;
  BOOST_TEST(t.at("k") == 6);
  BOOST_TEST(t.size() == 1);
}

BOOST_AUTO_TEST_CASE(sha1_known_vectors)
{
  const std::string abc = "abc";
  const uint8_t* data = reinterpret_cast< const uint8_t* >(abc.data());
  const gordejchik::sha1_digest_t digest = gordejchik::computeSha1(data, abc.size());
  const uint8_t expected[20] = {
    0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
    0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D,
  };
  for (size_t i = 0; i < sizeof(expected); ++i) {
    BOOST_TEST(digest.bytes[i] == expected[i]);
  }

  const std::string longInput = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
  const uint8_t* longData = reinterpret_cast< const uint8_t* >(longInput.data());
  const gordejchik::sha1_digest_t longDigest =
      gordejchik::computeSha1(longData, longInput.size());
  const uint8_t longExpected[20] = {
    0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
    0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1,
  };
  for (size_t i = 0; i < sizeof(longExpected); ++i) {
    BOOST_TEST(longDigest.bytes[i] == longExpected[i]);
  }
}
