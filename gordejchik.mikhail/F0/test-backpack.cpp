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

BOOST_AUTO_TEST_SUITE_END()
