#include "bsTree.hpp"
#include <boost/test/included/unit_test.hpp>
#include <string>

using gordejchik::BSTree;

BOOST_AUTO_TEST_SUITE(bstree_tests)

BOOST_AUTO_TEST_CASE(empty_tree)
{
  BSTree< int, std::string > tree;
  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_CHECK(tree.begin() == tree.end());
  BOOST_TEST(tree.height() == 0);
}

BOOST_AUTO_TEST_CASE(push_and_get)
{
  BSTree< int, std::string > tree;
  tree.push(5, "five");
  tree.push(3, "three");
  tree.push(7, "seven");
  BOOST_TEST(tree.size() == 3);
  BOOST_TEST(!tree.empty());
  BOOST_TEST(tree.get(5) == "five");
  BOOST_TEST(tree.get(3) == "three");
  BOOST_TEST(tree.get(7) == "seven");
}

BOOST_AUTO_TEST_CASE(push_updates_existing)
{
  BSTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(1, "uno");
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree.get(1) == "uno");
}

BOOST_AUTO_TEST_CASE(contains_and_find)
{
  BSTree< int, std::string > tree;
  tree.push(10, "ten");
  tree.push(20, "twenty");
  BOOST_TEST(tree.contains(10));
  BOOST_TEST(!tree.contains(15));
  BOOST_CHECK(tree.find(10) != tree.end());
  BOOST_CHECK(tree.find(99) == tree.end());
}

BOOST_AUTO_TEST_CASE(get_throws_on_missing)
{
  BSTree< int, std::string > tree;
  BOOST_CHECK_THROW(tree.get(1), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(operator_bracket)
{
  BSTree< int, std::string > tree;
  tree[5] = "five";
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree[5] == "five");
  tree[5] = "cinq";
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree[5] == "cinq");
}

BOOST_AUTO_TEST_CASE(forward_iteration_sorted)
{
  BSTree< int, std::string > tree;
  tree.push(30, "c");
  tree.push(10, "a");
  tree.push(20, "b");
  tree.push(40, "d");

  BSTree< int, std::string >::const_iterator it = tree.cbegin();
  BOOST_TEST(it->first == 10);
  ++it;
  BOOST_TEST(it->first == 20);
  ++it;
  BOOST_TEST(it->first == 30);
  ++it;
  BOOST_TEST(it->first == 40);
  ++it;
  BOOST_CHECK(it == tree.cend());
}

BOOST_AUTO_TEST_CASE(backward_iteration)
{
  BSTree< int, std::string > tree;
  tree.push(1, "a");
  tree.push(2, "b");
  tree.push(3, "c");

  BSTree< int, std::string >::const_iterator it = tree.cend();
  --it;
  BOOST_TEST(it->first == 3);
  --it;
  BOOST_TEST(it->first == 2);
  --it;
  BOOST_TEST(it->first == 1);
  BOOST_CHECK(it == tree.cbegin());
}

BOOST_AUTO_TEST_CASE(erase_leaf)
{
  BSTree< int, std::string > tree;
  tree.push(5, "five");
  tree.push(3, "three");
  tree.push(7, "seven");
  tree.erase(tree.find(3));
  BOOST_TEST(tree.size() == 2);
  BOOST_TEST(!tree.contains(3));
}

BOOST_AUTO_TEST_CASE(erase_one_child)
{
  BSTree< int, std::string > tree;
  tree.push(5, "five");
  tree.push(3, "three");
  tree.push(4, "four");
  tree.erase(tree.find(3));
  BOOST_TEST(tree.size() == 2);
  BOOST_TEST(tree.contains(4));
  BOOST_TEST(tree.contains(5));
}

BOOST_AUTO_TEST_CASE(erase_two_children)
{
  BSTree< int, std::string > tree;
  tree.push(5, "five");
  tree.push(3, "three");
  tree.push(7, "seven");
  tree.push(6, "six");
  tree.push(8, "eight");
  tree.erase(tree.find(7));
  BOOST_TEST(tree.size() == 4);
  BOOST_TEST(!tree.contains(7));
  BOOST_TEST(tree.contains(6));
  BOOST_TEST(tree.contains(8));
}

BOOST_AUTO_TEST_CASE(erase_root)
{
  BSTree< int, std::string > tree;
  tree.push(5, "five");
  tree.push(3, "three");
  tree.push(7, "seven");
  tree.erase(tree.find(5));
  BOOST_TEST(tree.size() == 2);
  BOOST_TEST(!tree.contains(5));
  BOOST_TEST(tree.contains(3));
  BOOST_TEST(tree.contains(7));
}

BOOST_AUTO_TEST_CASE(drop_returns_value)
{
  BSTree< int, std::string > tree;
  tree.push(1, "one");
  tree.push(2, "two");
  const std::string val = tree.drop(1);
  BOOST_TEST(val == "one");
  BOOST_TEST(tree.size() == 1);
  BOOST_CHECK_THROW(tree.drop(99), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(rotate_right)
{
  BSTree< int, std::string > tree;
  tree.push(10, "a");
  tree.push(5, "b");
  tree.push(15, "c");
  tree.push(3, "d");
  tree.push(7, "e");

  BSTree< int, std::string >::iterator it = tree.find(5);
  tree.rotateRight(it);
  BOOST_TEST(tree.cbegin()->first == 3);

  int prev = -1;
  for (BSTree< int, std::string >::const_iterator ci = tree.cbegin(); ci != tree.cend(); ++ci) {
    BOOST_TEST(ci->first > prev);
    prev = ci->first;
  }
}

BOOST_AUTO_TEST_CASE(rotate_left)
{
  BSTree< int, std::string > tree;
  tree.push(10, "a");
  tree.push(5, "b");
  tree.push(15, "c");
  tree.push(12, "d");
  tree.push(20, "e");

  BSTree< int, std::string >::iterator it = tree.find(15);
  tree.rotateLeft(it);

  int prev = -1;
  for (BSTree< int, std::string >::const_iterator ci = tree.cbegin(); ci != tree.cend(); ++ci) {
    BOOST_TEST(ci->first > prev);
    prev = ci->first;
  }
}

BOOST_AUTO_TEST_CASE(rotate_large_right)
{
  BSTree< int, std::string > tree;
  tree.push(10, "a");
  tree.push(5, "b");
  tree.push(7, "c");

  BSTree< int, std::string >::iterator it = tree.find(7);
  tree.rotateLargeRight(it);

  int prev = -1;
  for (BSTree< int, std::string >::const_iterator ci = tree.cbegin(); ci != tree.cend(); ++ci) {
    BOOST_TEST(ci->first > prev);
    prev = ci->first;
  }
  BOOST_TEST(tree.size() == 3);
}

BOOST_AUTO_TEST_CASE(rotate_large_left)
{
  BSTree< int, std::string > tree;
  tree.push(10, "a");
  tree.push(15, "b");
  tree.push(12, "c");

  BSTree< int, std::string >::iterator it = tree.find(12);
  tree.rotateLargeLeft(it);

  int prev = -1;
  for (BSTree< int, std::string >::const_iterator ci = tree.cbegin(); ci != tree.cend(); ++ci) {
    BOOST_TEST(ci->first > prev);
    prev = ci->first;
  }
  BOOST_TEST(tree.size() == 3);
}

BOOST_AUTO_TEST_CASE(height_test)
{
  BSTree< int, std::string > tree;
  BOOST_TEST(tree.height() == 0);
  tree.push(5, "a");
  BOOST_TEST(tree.height() == 1);
  tree.push(3, "b");
  tree.push(7, "c");
  BOOST_TEST(tree.height() == 2);
  tree.push(1, "d");
  BOOST_TEST(tree.height() == 3);
}

BOOST_AUTO_TEST_CASE(copy_constructor)
{
  BSTree< int, std::string > src;
  src.push(1, "a");
  src.push(2, "b");
  src.push(3, "c");

  BSTree< int, std::string > copy(src);
  BOOST_TEST(copy.size() == 3);
  BOOST_TEST(copy.get(1) == "a");
  BOOST_TEST(copy.get(2) == "b");
  BOOST_TEST(copy.get(3) == "c");

  copy.push(1, "z");
  BOOST_TEST(src.get(1) == "a");
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
  BSTree< int, std::string > src;
  src.push(1, "a");
  src.push(2, "b");

  BSTree< int, std::string > moved(static_cast< BSTree< int, std::string >&& >(src));
  BOOST_TEST(moved.size() == 2);
  BOOST_TEST(moved.get(1) == "a");
  BOOST_TEST(src.empty());
}

BOOST_AUTO_TEST_CASE(copy_assignment)
{
  BSTree< int, std::string > src;
  src.push(1, "a");
  src.push(2, "b");

  BSTree< int, std::string > dst;
  dst.push(99, "z");
  dst = src;
  BOOST_TEST(dst.size() == 2);
  BOOST_TEST(dst.get(1) == "a");
  BOOST_TEST(!dst.contains(99));
}

BOOST_AUTO_TEST_CASE(move_assignment)
{
  BSTree< int, std::string > src;
  src.push(1, "a");

  BSTree< int, std::string > dst;
  dst.push(99, "z");
  dst = static_cast< BSTree< int, std::string >&& >(src);
  BOOST_TEST(dst.size() == 1);
  BOOST_TEST(dst.get(1) == "a");
  BOOST_TEST(src.empty());
}

BOOST_AUTO_TEST_CASE(clear_test)
{
  BSTree< int, std::string > tree;
  tree.push(1, "a");
  tree.push(2, "b");
  tree.clear();
  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_CHECK(tree.begin() == tree.end());
}

BOOST_AUTO_TEST_SUITE_END()
