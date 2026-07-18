#include <utility>
#include <boost/test/unit_test.hpp>
#include "backpack.hpp"
#include "card.hpp"
#include "gameconfig.hpp"

using gordejchik::BackpackResult;
using gordejchik::Card;
using gordejchik::computeTypeBonuses;
using gordejchik::game_config_t;

static BackpackResult makeSide(Card* cards, size_t count)
{
  Card** ptrs = new Card*[count];
  for (size_t i = 0; i < count; ++i) {
    ptrs[i] = cards + i;
  }
  return BackpackResult(ptrs, count, 0);
}

BOOST_AUTO_TEST_SUITE(TypeBonusSuite)

BOOST_AUTO_TEST_CASE(bothSidesEmpty)
{
  game_config_t config{true, 3, {}};
  config.rules.insert("Demon>Beast", true);
  const BackpackResult left(nullptr, 0, 0);
  const BackpackResult right(nullptr, 0, 0);

  const std::pair< int, int > bonuses = computeTypeBonuses(left, right, config);
  BOOST_TEST(bonuses.first == 0);
  BOOST_TEST(bonuses.second == 0);
}

BOOST_AUTO_TEST_CASE(sameTypeGetsNoBonus)
{
  game_config_t config{true, 3, {}};
  config.rules.insert("Beast>Machine", true);
  Card left[] = {{"a", 1, 1, "Beast", ""}, {"b", 2, 1, "Beast", ""}};
  Card right[] = {{"c", 3, 1, "Beast", ""}};

  const std::pair< int, int > bonuses =
      computeTypeBonuses(makeSide(left, 2), makeSide(right, 1), config);
  BOOST_TEST(bonuses.first == 0);
  BOOST_TEST(bonuses.second == 0);
}

BOOST_AUTO_TEST_CASE(mixedTypesCountEveryPair)
{
  game_config_t config{true, 3, {}};
  config.rules.insert("Demon>Beast", true);
  config.rules.insert("Beast>Machine", true);
  config.rules.insert("Machine>Demon", true);
  Card left[] = {{"a", 1, 1, "Beast", ""}, {"b", 2, 1, "Demon", ""}};
  Card right[] = {{"c", 3, 1, "Machine", ""}, {"d", 4, 1, "Beast", ""}};

  const std::pair< int, int > bonuses =
      computeTypeBonuses(makeSide(left, 2), makeSide(right, 2), config);
  BOOST_TEST(bonuses.first == 6);
  BOOST_TEST(bonuses.second == 3);
}

BOOST_AUTO_TEST_CASE(untypedCardsAreSkipped)
{
  game_config_t config{true, 5, {}};
  config.rules.insert("Beast>Machine", true);
  Card left[] = {{"a", 1, 1, "Beast", ""}, {"b", 2, 1, "", ""}};
  Card right[] = {{"c", 3, 1, "Machine", ""}, {"d", 4, 1, "", ""}};

  const std::pair< int, int > bonuses =
      computeTypeBonuses(makeSide(left, 2), makeSide(right, 2), config);
  BOOST_TEST(bonuses.first == 5);
  BOOST_TEST(bonuses.second == 0);
}

BOOST_AUTO_TEST_CASE(disabledSystemGivesZero)
{
  game_config_t config{false, 3, {}};
  config.rules.insert("Beast>Machine", true);
  Card left[] = {{"a", 1, 1, "Beast", ""}};
  Card right[] = {{"c", 3, 1, "Machine", ""}};

  const std::pair< int, int > bonuses =
      computeTypeBonuses(makeSide(left, 1), makeSide(right, 1), config);
  BOOST_TEST(bonuses.first == 0);
  BOOST_TEST(bonuses.second == 0);
}

BOOST_AUTO_TEST_CASE(noRulesGivesZero)
{
  game_config_t config{true, 3, {}};
  Card left[] = {{"a", 1, 1, "Beast", ""}};
  Card right[] = {{"c", 3, 1, "Machine", ""}};

  const std::pair< int, int > bonuses =
      computeTypeBonuses(makeSide(left, 1), makeSide(right, 1), config);
  BOOST_TEST(bonuses.first == 0);
  BOOST_TEST(bonuses.second == 0);
}

BOOST_AUTO_TEST_CASE(bonusValueScalesResult)
{
  game_config_t config{true, 7, {}};
  config.rules.insert("Beast>Machine", true);
  Card left[] = {{"a", 1, 1, "Beast", ""}};
  Card right[] = {{"c", 3, 1, "Machine", ""}, {"d", 4, 1, "Machine", ""}};

  const std::pair< int, int > bonuses =
      computeTypeBonuses(makeSide(left, 1), makeSide(right, 2), config);
  BOOST_TEST(bonuses.first == 14);
  BOOST_TEST(bonuses.second == 0);
}

BOOST_AUTO_TEST_SUITE_END()
