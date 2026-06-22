#define BOOST_TEST_MODULE BackpackTest
#include <boost/test/included/unit_test.hpp>
#include "backpack.hpp"
#include <string>

using gordejchik::Card;
using gordejchik::BackpackResult;
using gordejchik::solveBackpack;
using gordejchik::freeBackpackResult;

BOOST_AUTO_TEST_SUITE(BackpackSuite)

BOOST_AUTO_TEST_CASE(emptyDeck)
{
  BackpackResult res = solveBackpack(nullptr, 0, 10);
  BOOST_TEST(res.count_ == 0u);
  BOOST_TEST(res.totalPower_ == 0);
}

BOOST_AUTO_TEST_CASE(zeroBudget)
{
  Card c{"a", 10, 5, "", ""};
  Card* arr[] = {&c};
  BackpackResult res = solveBackpack(arr, 1, 0);
  BOOST_TEST(res.count_ == 0u);
  BOOST_TEST(res.totalPower_ == 0);
}

BOOST_AUTO_TEST_CASE(singleCardFits)
{
  Card c{"sword", 15, 3, "", ""};
  Card* arr[] = {&c};
  BackpackResult res = solveBackpack(arr, 1, 5);
  BOOST_TEST(res.count_ == 1u);
  BOOST_TEST(res.totalPower_ == 15);
  BOOST_TEST(res.cards_[0]->name_ == "sword");
  freeBackpackResult(res);
}

BOOST_AUTO_TEST_CASE(singleCardTooExpensive)
{
  Card c{"dragon", 100, 50, "", ""};
  Card* arr[] = {&c};
  BackpackResult res = solveBackpack(arr, 1, 10);
  BOOST_TEST(res.count_ == 0u);
  BOOST_TEST(res.totalPower_ == 0);
}

BOOST_AUTO_TEST_CASE(classicBackpack)
{
  Card c1{"a", 6, 1, "", ""};
  Card c2{"b", 10, 2, "", ""};
  Card c3{"c", 12, 3, "", ""};
  Card* arr[] = {&c1, &c2, &c3};
  BackpackResult res = solveBackpack(arr, 3, 5);

  BOOST_TEST(res.totalPower_ == 22);
  BOOST_TEST(res.count_ == 2u);
  freeBackpackResult(res);
}

BOOST_AUTO_TEST_CASE(allCardsFit)
{
  Card c1{"x", 5, 2, "", ""};
  Card c2{"y", 3, 1, "", ""};
  Card c3{"z", 7, 3, "", ""};
  Card* arr[] = {&c1, &c2, &c3};
  BackpackResult res = solveBackpack(arr, 3, 100);

  BOOST_TEST(res.count_ == 3u);
  BOOST_TEST(res.totalPower_ == 15);
  freeBackpackResult(res);
}

BOOST_AUTO_TEST_CASE(greedyWouldFail)
{
  Card c1{"cheap", 8, 4, "", ""};
  Card c2{"mid", 9, 5, "", ""};
  Card c3{"combo1", 5, 3, "", ""};
  Card c4{"combo2", 5, 3, "", ""};
  Card* arr[] = {&c1, &c2, &c3, &c4};
  BackpackResult res = solveBackpack(arr, 4, 6);

  BOOST_TEST(res.totalPower_ == 10);
  BOOST_TEST(res.count_ == 2u);
  freeBackpackResult(res);
}

BOOST_AUTO_TEST_SUITE_END()
