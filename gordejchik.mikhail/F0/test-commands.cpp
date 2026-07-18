#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/test/unit_test.hpp>
#include "commands.hpp"
#include "gameconfig.hpp"

using gordejchik::DeckStore;
using gordejchik::ParsedCommand;
using gordejchik::parseLine;

BOOST_AUTO_TEST_SUITE(ParseLineSuite)

BOOST_AUTO_TEST_CASE(plainTokens)
{
  ParsedCommand cmd = parseLine("add deck card 1 2");
  BOOST_TEST(cmd.count == 5u);
  BOOST_TEST(cmd.tokens[0] == "add");
  BOOST_TEST(cmd.tokens[4] == "2");
}

BOOST_AUTO_TEST_CASE(quotedTokenKeepsSpaces)
{
  ParsedCommand cmd = parseLine("add d \"two words\" 7 3");
  BOOST_TEST(cmd.count == 5u);
  BOOST_TEST(cmd.tokens[2] == "two words");
}

BOOST_AUTO_TEST_CASE(repeatedSpaces)
{
  ParsedCommand cmd = parseLine("  show   d  ");
  BOOST_TEST(cmd.count == 2u);
  BOOST_TEST(cmd.tokens[0] == "show");
  BOOST_TEST(cmd.tokens[1] == "d");
}

BOOST_AUTO_TEST_CASE(emptyLine)
{
  ParsedCommand cmd = parseLine("");
  BOOST_TEST(cmd.count == 0u);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CommandSuite)

BOOST_AUTO_TEST_CASE(addRejectsNegativeCost)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdCreate(decks, config, parseLine("create d"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add d cursed 10 -5"), out);

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(!decks.at("d").contains("cursed"));
}

BOOST_AUTO_TEST_CASE(optimizePicksBestSubset)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdCreate(decks, config, parseLine("create d"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add d b 10 2"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add d a 6 1"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add d c 12 3"), out);
  gordejchik::cmdOptimize(decks, config, parseLine("optimize d 5"), out);

  const std::string expected =
      "b: POWER 10, COST 2\n"
      "c: POWER 12, COST 3\n"
      "TOTAL POWER: 22\n";
  BOOST_TEST(out.str() == expected);
}

BOOST_AUTO_TEST_CASE(saveLoadRoundTrip)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdCreate(decks, config, parseLine("create d"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add d \"two words\" 7 3"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add d sword 5 2"), out);
  gordejchik::cmdSetType(decks, config, parseLine("set-type d sword weapon"), out);
  gordejchik::cmdSetDesc(decks, config,
      parseLine("set-desc d sword \"a sharp blade\""), out);

  const std::string filename = "f0-roundtrip-test.txt";
  gordejchik::cmdSave(decks, config, parseLine("save d " + filename), out);
  gordejchik::cmdLoad(decks, config, parseLine("load d2 " + filename), out);
  std::remove(filename.c_str());

  BOOST_TEST(out.str().empty());
  BOOST_REQUIRE(decks.contains("d2"));
  const gordejchik::Deck& copy = decks.at("d2");
  BOOST_TEST(copy.size() == 2u);
  BOOST_TEST(copy.at("two words").power == 7);
  BOOST_TEST(copy.at("two words").cost == 3);
  BOOST_TEST(copy.at("sword").type == "weapon");
  BOOST_TEST(copy.at("sword").description == "a sharp blade");
}

BOOST_AUTO_TEST_CASE(loadRejectsNegativeCost)
{
  const std::string filename = "f0-negcost-test.txt";
  {
    std::ofstream file(filename);
    file << "1\ncursed\n10 -5\n\n\n";
  }
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdLoad(decks, config, parseLine("load d " + filename), out);
  std::remove(filename.c_str());

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(!decks.contains("d"));
}

BOOST_AUTO_TEST_SUITE_END()

static DeckStore makeTradeStore()
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdCreate(decks, config, parseLine("create alpha"), out);
  gordejchik::cmdCreate(decks, config, parseLine("create beta"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add alpha sword 10 3"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add alpha bow 7 2"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add beta axe 8 2"), out);
  gordejchik::cmdSetType(decks, config, parseLine("set-type alpha sword Blade"), out);
  return decks;
}

BOOST_AUTO_TEST_SUITE(TradeSuite)

BOOST_AUTO_TEST_CASE(swapsCardsBetweenDecks)
{
  DeckStore decks = makeTradeStore();
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdTrade(decks, config, parseLine("trade alpha beta sword axe"), out);

  BOOST_TEST(out.str().empty());
  BOOST_TEST(!decks.at("alpha").contains("sword"));
  BOOST_TEST(decks.at("alpha").contains("axe"));
  BOOST_TEST(!decks.at("beta").contains("axe"));
  BOOST_REQUIRE(decks.at("beta").contains("sword"));
  BOOST_TEST(decks.at("beta").at("sword").power == 10);
  BOOST_TEST(decks.at("beta").at("sword").cost == 3);
  BOOST_TEST(decks.at("beta").at("sword").type == "Blade");
}

BOOST_AUTO_TEST_CASE(missingDeckRejected)
{
  DeckStore decks = makeTradeStore();
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdTrade(decks, config, parseLine("trade nosuch beta sword axe"), out);
  gordejchik::cmdTrade(decks, config, parseLine("trade alpha nosuch sword axe"), out);

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n<INVALID COMMAND>\n");
  BOOST_TEST(decks.at("alpha").contains("sword"));
  BOOST_TEST(decks.at("beta").contains("axe"));
}

BOOST_AUTO_TEST_CASE(missingCardRejected)
{
  DeckStore decks = makeTradeStore();
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdTrade(decks, config, parseLine("trade alpha beta nosuch axe"), out);
  gordejchik::cmdTrade(decks, config, parseLine("trade alpha beta sword nosuch"), out);

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n<INVALID COMMAND>\n");
  BOOST_TEST(decks.at("alpha").contains("sword"));
  BOOST_TEST(decks.at("beta").contains("axe"));
}

BOOST_AUTO_TEST_CASE(nameCollisionInTargetRejected)
{
  DeckStore decks = makeTradeStore();
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdAdd(decks, config, parseLine("add beta sword 1 1"), out);
  gordejchik::cmdTrade(decks, config, parseLine("trade alpha beta sword axe"), out);

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(decks.at("alpha").contains("sword"));
  BOOST_TEST(decks.at("beta").contains("axe"));
}

BOOST_AUTO_TEST_CASE(nameCollisionInSourceRejected)
{
  DeckStore decks = makeTradeStore();
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdAdd(decks, config, parseLine("add alpha axe 1 1"), out);
  gordejchik::cmdTrade(decks, config, parseLine("trade alpha beta sword axe"), out);

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(decks.at("alpha").contains("sword"));
  BOOST_TEST(decks.at("beta").contains("axe"));
}

BOOST_AUTO_TEST_CASE(sameDeckSwapKeepsCards)
{
  DeckStore decks = makeTradeStore();
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdTrade(decks, config, parseLine("trade alpha alpha sword bow"), out);

  BOOST_TEST(out.str().empty());
  BOOST_TEST(decks.at("alpha").size() == 2u);
  BOOST_TEST(decks.at("alpha").contains("sword"));
  BOOST_TEST(decks.at("alpha").contains("bow"));
}

BOOST_AUTO_TEST_CASE(sameDeckSameCardRejected)
{
  DeckStore decks = makeTradeStore();
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdTrade(decks, config, parseLine("trade alpha alpha sword sword"), out);

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(decks.at("alpha").contains("sword"));
}

BOOST_AUTO_TEST_CASE(wrongArgumentCountRejected)
{
  DeckStore decks = makeTradeStore();
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdTrade(decks, config, parseLine("trade alpha beta sword"), out);

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ConfigRuleSuite)

BOOST_AUTO_TEST_CASE(emptyTypeRejected)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdConfig(decks, config, parseLine("config rule \"\" Beast"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config rule Beast \"\""), out);

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n<INVALID COMMAND>\n");
  BOOST_TEST(config.rules.empty());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ConfigAnalyzeSuite)

BOOST_AUTO_TEST_CASE(analyzeWithoutRules)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdConfig(decks, config, parseLine("config analyze"), out);

  BOOST_TEST(out.str() == "No rules to analyze\n");
}

BOOST_AUTO_TEST_CASE(analyzeRanksAcyclicRules)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdConfig(decks, config, parseLine("config rule Demon Beast"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config rule Beast Machine"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config analyze"), out);

  const std::string expected =
      "Type ranking:\n"
      "  1. Demon\n"
      "  2. Beast\n"
      "  3. Machine\n";
  BOOST_TEST(out.str() == expected);
}

BOOST_AUTO_TEST_CASE(analyzeReportsCycle)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdConfig(decks, config, parseLine("config rule Demon Beast"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config rule Beast Demon"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config analyze"), out);

  const std::string text = out.str();
  BOOST_TEST(text.substr(0, 7) == "Cycle: ");
  BOOST_TEST(text.find("Demon") != std::string::npos);
  BOOST_TEST(text.find("Beast") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(dominatesListsChain)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdConfig(decks, config, parseLine("config rule Demon Beast"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config rule Beast Machine"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config dominates Demon"), out);

  BOOST_TEST(out.str() == "Demon dominates: Beast Machine\n");
}

BOOST_AUTO_TEST_CASE(dominatesUnknownType)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdConfig(decks, config, parseLine("config dominates Ghost"), out);

  BOOST_TEST(out.str() == "Ghost dominates nothing\n");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SessionSuite)

BOOST_AUTO_TEST_CASE(sessionRoundTripRestoresState)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdCreate(decks, config, parseLine("create \"Light Side\""), out);
  gordejchik::cmdAdd(decks, config, parseLine("add \"Light Side\" Yoda 10 8"), out);
  gordejchik::cmdSetType(decks, config, parseLine("set-type \"Light Side\" Yoda Jedi"), out);
  gordejchik::cmdCreate(decks, config, parseLine("create empty"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config bonus on"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config bonus 5"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config rule Jedi Sith"), out);

  const std::string filename = "f0-session-test.txt";
  gordejchik::cmdSave(decks, config, parseLine("save " + filename), out);

  DeckStore restoredDecks;
  gordejchik::game_config_t restored{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  gordejchik::cmdLoad(restoredDecks, restored, parseLine("load " + filename), out);
  std::remove(filename.c_str());

  BOOST_TEST(out.str().empty());
  BOOST_TEST(restored.bonusEnabled);
  BOOST_TEST(restored.bonusValue == 5);
  BOOST_TEST(restored.rules.size() == 1u);
  BOOST_TEST(restored.rules.contains("Jedi>Sith"));
  BOOST_TEST(restoredDecks.size() == 2u);
  BOOST_REQUIRE(restoredDecks.contains("Light Side"));
  BOOST_TEST(restoredDecks.contains("empty"));
  BOOST_TEST(restoredDecks.at("empty").empty());
  const gordejchik::Deck& deck = restoredDecks.at("Light Side");
  BOOST_REQUIRE(deck.contains("Yoda"));
  BOOST_TEST(deck.at("Yoda").power == 10);
  BOOST_TEST(deck.at("Yoda").cost == 8);
  BOOST_TEST(deck.at("Yoda").type == "Jedi");
}

BOOST_AUTO_TEST_CASE(sessionLoadReplacesState)
{
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  const std::string filename = "f0-replace-test.txt";
  gordejchik::cmdSave(decks, config, parseLine("save " + filename), out);

  gordejchik::cmdCreate(decks, config, parseLine("create old"), out);
  gordejchik::cmdConfig(decks, config, parseLine("config rule Old Rule"), out);
  gordejchik::cmdLoad(decks, config, parseLine("load " + filename), out);
  std::remove(filename.c_str());

  BOOST_TEST(out.str().empty());
  BOOST_TEST(decks.empty());
  BOOST_TEST(config.rules.empty());
}

BOOST_AUTO_TEST_CASE(corruptedSessionKeepsState)
{
  const std::string filename = "f0-corrupt-test.txt";
  {
    std::ofstream file(filename);
    file << "SESSION\non -3\n0\n0\n";
  }
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdCreate(decks, config, parseLine("create keepme"), out);
  gordejchik::cmdLoad(decks, config, parseLine("load " + filename), out);
  std::remove(filename.c_str());

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(decks.contains("keepme"));
  BOOST_TEST(!config.bonusEnabled);
}

BOOST_AUTO_TEST_CASE(deckFileRejectedAsSession)
{
  const std::string filename = "f0-deckfile-test.txt";
  DeckStore decks;
  gordejchik::game_config_t config{false, gordejchik::DEFAULT_BONUS_VALUE, {}};
  std::ostringstream out;
  gordejchik::cmdCreate(decks, config, parseLine("create d"), out);
  gordejchik::cmdAdd(decks, config, parseLine("add d Yoda 10 8"), out);
  gordejchik::cmdSave(decks, config, parseLine("save d " + filename), out);

  gordejchik::cmdLoad(decks, config, parseLine("load " + filename), out);
  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(decks.contains("d"));

  DeckStore other;
  std::ostringstream out2;
  gordejchik::cmdSave(other, config, parseLine("save " + filename), out2);
  gordejchik::cmdLoad(other, config, parseLine("load s " + filename), out2);
  std::remove(filename.c_str());
  BOOST_TEST(out2.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(!other.contains("s"));
}

BOOST_AUTO_TEST_SUITE_END()
