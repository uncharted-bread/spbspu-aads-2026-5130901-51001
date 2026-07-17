#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/test/unit_test.hpp>
#include "commands.hpp"

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
  std::ostringstream out;
  gordejchik::cmdCreate(decks, parseLine("create d"), out);
  gordejchik::cmdAdd(decks, parseLine("add d cursed 10 -5"), out);

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(!decks.at("d").contains("cursed"));
}

BOOST_AUTO_TEST_CASE(optimizePicksBestSubset)
{
  DeckStore decks;
  std::ostringstream out;
  gordejchik::cmdCreate(decks, parseLine("create d"), out);
  gordejchik::cmdAdd(decks, parseLine("add d b 10 2"), out);
  gordejchik::cmdAdd(decks, parseLine("add d a 6 1"), out);
  gordejchik::cmdAdd(decks, parseLine("add d c 12 3"), out);
  gordejchik::cmdOptimize(decks, parseLine("optimize d 5"), out);

  const std::string expected =
      "b: POWER 10, COST 2\n"
      "c: POWER 12, COST 3\n"
      "TOTAL POWER: 22\n";
  BOOST_TEST(out.str() == expected);
}

BOOST_AUTO_TEST_CASE(saveLoadRoundTrip)
{
  DeckStore decks;
  std::ostringstream out;
  gordejchik::cmdCreate(decks, parseLine("create d"), out);
  gordejchik::cmdAdd(decks, parseLine("add d \"two words\" 7 3"), out);
  gordejchik::cmdAdd(decks, parseLine("add d sword 5 2"), out);
  gordejchik::cmdSetType(decks, parseLine("set-type d sword weapon"), out);
  gordejchik::cmdSetDesc(decks,
      parseLine("set-desc d sword \"a sharp blade\""), out);

  const std::string filename = "f0-roundtrip-test.txt";
  gordejchik::cmdSave(decks, parseLine("save d " + filename), out);
  gordejchik::cmdLoad(decks, parseLine("load d2 " + filename), out);
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
  std::ostringstream out;
  gordejchik::cmdLoad(decks, parseLine("load d " + filename), out);
  std::remove(filename.c_str());

  BOOST_TEST(out.str() == "<INVALID COMMAND>\n");
  BOOST_TEST(!decks.contains("d"));
}

BOOST_AUTO_TEST_SUITE_END()
