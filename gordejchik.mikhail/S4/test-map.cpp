#include <string>
#include <utility>
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include "map.hpp"

using gordejchik::Map;

BOOST_AUTO_TEST_SUITE(map_tests)

BOOST_AUTO_TEST_CASE(empty_map)
{
  Map< int, std::string > map;
  BOOST_TEST(map.empty());
  BOOST_TEST(map.size() == 0);
  BOOST_TEST(map.count(1) == 0);
  BOOST_CHECK(map.begin() == map.end());
}

BOOST_AUTO_TEST_CASE(insert_and_find)
{
  using MapT = Map< int, std::string >;
  MapT map;
  std::pair< MapT::iterator, bool > res = map.insert(MapT::value_type(1, "one"));
  BOOST_TEST(res.second);
  BOOST_TEST(res.first->second == "one");
  BOOST_CHECK(map.find(1) != map.end());
  BOOST_CHECK(map.find(2) == map.end());

  std::pair< MapT::iterator, bool > dup = map.insert(MapT::value_type(1, "uno"));
  BOOST_TEST(!dup.second);
  BOOST_TEST(map.at(1) == "one");
  BOOST_TEST(map.size() == 1);
}

BOOST_AUTO_TEST_CASE(operator_bracket)
{
  Map< int, std::string > map;
  map[5] = "five";
  BOOST_TEST(map.size() == 1);
  BOOST_TEST(map[5] == "five");
  map[5] = "cinq";
  BOOST_TEST(map.size() == 1);
  BOOST_TEST(map[5] == "cinq");
}

BOOST_AUTO_TEST_CASE(at_throws_on_missing)
{
  Map< int, std::string > map;
  map.insert(Map< int, std::string >::value_type(1, "one"));
  BOOST_TEST(map.at(1) == "one");
  BOOST_CHECK_THROW(map.at(99), std::out_of_range);

  const Map< int, std::string >& constMap = map;
  BOOST_TEST(constMap.at(1) == "one");
  BOOST_CHECK_THROW(constMap.at(99), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(count_reports_presence)
{
  Map< std::string, int > map;
  map["one"] = 1;
  BOOST_TEST(map.count("one") == 1);
  BOOST_TEST(map.count("two") == 0);
}

BOOST_AUTO_TEST_CASE(erase_by_key)
{
  Map< int, std::string > map;
  map[1] = "a";
  map[2] = "b";
  BOOST_TEST(map.erase(1) == 1);
  BOOST_TEST(map.size() == 1);
  BOOST_TEST(map.count(1) == 0);
  BOOST_TEST(map.erase(1) == 0);
}

BOOST_AUTO_TEST_CASE(erase_by_iterator)
{
  Map< int, std::string > map;
  map[1] = "a";
  map[2] = "b";
  map[3] = "c";
  Map< int, std::string >::iterator next = map.erase(map.find(2));
  BOOST_TEST(next->first == 3);
  BOOST_TEST(map.size() == 2);
}

BOOST_AUTO_TEST_CASE(iteration_is_sorted)
{
  Map< int, std::string > map;
  const int count = 100;
  for (int i = count; i > 0; --i) {
    map[i] = "v";
  }
  BOOST_TEST(map.size() == static_cast< size_t >(count));
  int prev = 0;
  size_t seen = 0;
  Map< int, std::string >::const_iterator it = map.cbegin();
  for (; it != map.cend(); ++it) {
    BOOST_TEST(it->first > prev);
    prev = it->first;
    ++seen;
  }
  BOOST_TEST(seen == map.size());
}

BOOST_AUTO_TEST_CASE(copy_is_deep)
{
  Map< int, std::string > src;
  src[1] = "a";
  src[2] = "b";

  Map< int, std::string > copy(src);
  BOOST_TEST(copy.size() == 2);
  copy[1] = "z";
  BOOST_TEST(src.at(1) == "a");
}

BOOST_AUTO_TEST_CASE(swap_maps)
{
  Map< int, std::string > a;
  a[1] = "a";
  a[2] = "b";
  Map< int, std::string > b;
  b[10] = "x";

  a.swap(b);
  BOOST_TEST(a.size() == 1);
  BOOST_TEST(a.at(10) == "x");
  BOOST_TEST(b.size() == 2);
  BOOST_TEST(b.at(1) == "a");
}

BOOST_AUTO_TEST_CASE(clear_empties_map)
{
  Map< int, std::string > map;
  map[1] = "a";
  map[2] = "b";
  map.clear();
  BOOST_TEST(map.empty());
  BOOST_TEST(map.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
