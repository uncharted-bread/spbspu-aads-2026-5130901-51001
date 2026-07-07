#include <string>
#include <utility>
#include <boost/test/unit_test.hpp>
#include "backpack.hpp"

using gordejchik::Card;
using gordejchik::BackpackResult;
using gordejchik::solveBackpack;

BOOST_AUTO_TEST_SUITE(BackpackSuite)

BOOST_AUTO_TEST_CASE(emptyDeck)
{
  BackpackResult res = solveBackpack(nullptr, 0, 10);
  BOOST_TEST(res.count() == 0u);
  BOOST_TEST(res.totalPower() == 0);
}

BOOST_AUTO_TEST_CASE(zeroBudget)
{
  Card c{"a", 10, 5, "", ""};
  Card* arr[] = {&c};
  BackpackResult res = solveBackpack(arr, 1, 0);
  BOOST_TEST(res.count() == 0u);
  BOOST_TEST(res.totalPower() == 0);
}

BOOST_AUTO_TEST_CASE(singleCardFits)
{
  Card c{"sword", 15, 3, "", ""};
  Card* arr[] = {&c};
  BackpackResult res = solveBackpack(arr, 1, 5);
  BOOST_TEST(res.count() == 1u);
  BOOST_TEST(res.totalPower() == 15);
  BOOST_TEST(res.cards()[0]->name == "sword");
}

BOOST_AUTO_TEST_CASE(singleCardTooExpensive)
{
  Card c{"dragon", 100, 50, "", ""};
  Card* arr[] = {&c};
  BackpackResult res = solveBackpack(arr, 1, 10);
  BOOST_TEST(res.count() == 0u);
  BOOST_TEST(res.totalPower() == 0);
}

BOOST_AUTO_TEST_CASE(classicBackpack)
{
  Card c1{"a", 6, 1, "", ""};
  Card c2{"b", 10, 2, "", ""};
  Card c3{"c", 12, 3, "", ""};
  Card* arr[] = {&c1, &c2, &c3};
  BackpackResult res = solveBackpack(arr, 3, 5);

  BOOST_TEST(res.totalPower() == 22);
  BOOST_TEST(res.count() == 2u);
}

BOOST_AUTO_TEST_CASE(allCardsFit)
{
  Card c1{"x", 5, 2, "", ""};
  Card c2{"y", 3, 1, "", ""};
  Card c3{"z", 7, 3, "", ""};
  Card* arr[] = {&c1, &c2, &c3};
  BackpackResult res = solveBackpack(arr, 3, 100);

  BOOST_TEST(res.count() == 3u);
  BOOST_TEST(res.totalPower() == 15);
}

BOOST_AUTO_TEST_CASE(moveTransfersOwnership)
{
  Card c{"sword", 15, 3, "", ""};
  Card* arr[] = {&c};
  BackpackResult res = solveBackpack(arr, 1, 5);
  BackpackResult moved = std::move(res);

  BOOST_TEST(moved.count() == 1u);
  BOOST_TEST(moved.totalPower() == 15);
  BOOST_TEST(moved.cards()[0]->name == "sword");
  BOOST_TEST(res.count() == 0u);
  BOOST_TEST(res.totalPower() == 0);
}

BOOST_AUTO_TEST_CASE(greedyWouldFail)
{
  Card c1{"cheap", 8, 4, "", ""};
  Card c2{"mid", 9, 5, "", ""};
  Card c3{"combo1", 5, 3, "", ""};
  Card c4{"combo2", 5, 3, "", ""};
  Card* arr[] = {&c1, &c2, &c3, &c4};
  BackpackResult res = solveBackpack(arr, 4, 6);

  BOOST_TEST(res.totalPower() == 10);
  BOOST_TEST(res.count() == 2u);
}

BOOST_AUTO_TEST_SUITE_END()
