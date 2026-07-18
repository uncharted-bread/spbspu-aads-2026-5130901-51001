#include <string>
#include <utility>
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include "avlTree.hpp"

using gordejchik::AVLTree;

BOOST_AUTO_TEST_SUITE(avltree_tests)

BOOST_AUTO_TEST_CASE(empty_tree)
{
  AVLTree< int, std::string > tree;
  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_TEST(tree.height() == 0);
  BOOST_CHECK(tree.begin() == tree.end());
}

BOOST_AUTO_TEST_CASE(push_and_get)
{
  AVLTree< int, std::string > tree;
  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");
  BOOST_TEST(tree.size() == 3);
  BOOST_TEST(tree.get(1) == "one");
  BOOST_TEST(tree.get(2) == "two");
  BOOST_TEST(tree.get(3) == "three");
}

BOOST_AUTO_TEST_CASE(push_updates_existing)
{
  AVLTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(1, "uno");
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree.get(1) == "uno");
}

BOOST_AUTO_TEST_CASE(get_throws_on_missing)
{
  AVLTree< int, std::string > tree;
  BOOST_CHECK_THROW(tree.get(1), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(ascending_insertion_stays_balanced)
{
  AVLTree< int, std::string > tree;
  const int count = 100;
  const size_t maxAvlHeight = 9;
  for (int i = 0; i < count; ++i) {
    tree.push(i, "v");
  }
  BOOST_TEST(tree.size() == static_cast< size_t >(count));
  BOOST_TEST(tree.height() <= maxAvlHeight);
}

BOOST_AUTO_TEST_CASE(descending_insertion_stays_balanced)
{
  AVLTree< int, std::string > tree;
  const int count = 100;
  const size_t maxAvlHeight = 9;
  for (int i = count; i > 0; --i) {
    tree.push(i, "v");
  }
  BOOST_TEST(tree.size() == static_cast< size_t >(count));
  BOOST_TEST(tree.height() <= maxAvlHeight);
}

BOOST_AUTO_TEST_CASE(iteration_is_sorted)
{
  AVLTree< int, std::string > tree;
  tree.push(50, "a");
  tree.push(20, "b");
  tree.push(80, "c");
  tree.push(10, "d");
  tree.push(30, "e");
  tree.push(70, "f");
  tree.push(90, "g");

  int prev = 0;
  size_t seen = 0;
  AVLTree< int, std::string >::const_iterator it = tree.cbegin();
  for (; it != tree.cend(); ++it) {
    BOOST_TEST(it->first > prev);
    prev = it->first;
    ++seen;
  }
  BOOST_TEST(seen == tree.size());
}

BOOST_AUTO_TEST_CASE(backward_iteration)
{
  AVLTree< int, std::string > tree;
  tree.push(1, "a");
  tree.push(2, "b");
  tree.push(3, "c");

  AVLTree< int, std::string >::const_iterator it = tree.cend();
  --it;
  BOOST_TEST(it->first == 3);
  --it;
  BOOST_TEST(it->first == 2);
  --it;
  BOOST_TEST(it->first == 1);
  BOOST_CHECK(it == tree.cbegin());
}

BOOST_AUTO_TEST_CASE(insert_reports_result)
{
  using TreeT = AVLTree< int, std::string >;
  TreeT tree;
  std::pair< TreeT::iterator, bool > res = tree.insert(TreeT::value_type(1, "one"));
  BOOST_TEST(res.second);
  BOOST_TEST(res.first->first == 1);
  BOOST_CHECK(res.first == tree.find(1));

  std::pair< TreeT::iterator, bool > dup = tree.insert(TreeT::value_type(1, "uno"));
  BOOST_TEST(!dup.second);
  BOOST_TEST(tree.get(1) == "one");
  BOOST_TEST(tree.size() == 1);
}

BOOST_AUTO_TEST_CASE(erase_keeps_balance)
{
  AVLTree< int, std::string > tree;
  const int count = 100;
  const size_t maxAvlHeight = 7;
  for (int i = 0; i < count; ++i) {
    tree.push(i, "v");
  }
  for (int i = 0; i < count; i += 2) {
    tree.erase(tree.find(i));
  }
  BOOST_TEST(tree.size() == static_cast< size_t >(count / 2));
  BOOST_TEST(tree.height() <= maxAvlHeight);
  for (int i = 0; i < count; ++i) {
    const bool expected = (i % 2) == 1;
    BOOST_TEST(tree.contains(i) == expected);
  }
}

BOOST_AUTO_TEST_CASE(erase_returns_next)
{
  AVLTree< int, std::string > tree;
  tree.push(1, "a");
  tree.push(2, "b");
  tree.push(3, "c");
  tree.push(4, "d");

  AVLTree< int, std::string >::iterator next = tree.erase(tree.find(2));
  BOOST_TEST(next->first == 3);
  BOOST_TEST(tree.size() == 3);
  BOOST_TEST(!tree.contains(2));
}

BOOST_AUTO_TEST_CASE(drop_returns_value)
{
  AVLTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(2, "two");
  const std::string val = tree.drop(1);
  BOOST_TEST(val == "one");
  BOOST_TEST(tree.size() == 1);
  BOOST_CHECK_THROW(tree.drop(99), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(operator_bracket)
{
  AVLTree< int, std::string > tree;
  tree[5] = "five";
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree[5] == "five");
  tree[5] = "cinq";
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree[5] == "cinq");
}

BOOST_AUTO_TEST_CASE(copy_is_deep)
{
  AVLTree< int, std::string > src;
  src.push(1, "a");
  src.push(2, "b");
  src.push(3, "c");

  AVLTree< int, std::string > copy(src);
  BOOST_TEST(copy.size() == 3);
  BOOST_TEST(copy.get(2) == "b");
  copy.push(1, "z");
  BOOST_TEST(src.get(1) == "a");
}

BOOST_AUTO_TEST_CASE(move_transfers_contents)
{
  AVLTree< int, std::string > src;
  src.push(1, "a");
  src.push(2, "b");

  AVLTree< int, std::string > moved(std::move(src));
  BOOST_TEST(moved.size() == 2);
  BOOST_TEST(moved.get(1) == "a");
  BOOST_TEST(src.empty());
}

BOOST_AUTO_TEST_CASE(swap_trees)
{
  AVLTree< int, std::string > a;
  a.push(1, "a");
  a.push(2, "b");
  AVLTree< int, std::string > b;
  b.push(10, "x");

  a.swap(b);
  BOOST_TEST(a.size() == 1);
  BOOST_TEST(a.get(10) == "x");
  BOOST_TEST(b.size() == 2);
  BOOST_TEST(b.get(1) == "a");
}

BOOST_AUTO_TEST_CASE(clear_empties_tree)
{
  AVLTree< int, std::string > tree;
  tree.push(1, "a");
  tree.push(2, "b");
  tree.clear();
  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.height() == 0);
  BOOST_CHECK(tree.begin() == tree.end());
}

BOOST_AUTO_TEST_SUITE_END()
