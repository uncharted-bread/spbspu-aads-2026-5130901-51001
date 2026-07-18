#include <set>
#include <string>
#include <boost/test/unit_test.hpp>
#include "gameconfig.hpp"
#include "rulegraph.hpp"

using gordejchik::GameRules;
using gordejchik::RuleGraph;

static size_t findPosition(const RuleGraph& graph, const size_t* order,
    const std::string& name)
{
  size_t position = graph.vertexCount();
  for (size_t i = 0; i < graph.vertexCount(); ++i) {
    if (graph.vertexName(order[i]) == name) {
      position = i;
    }
  }
  return position;
}

BOOST_AUTO_TEST_SUITE(RuleGraphSuite)

BOOST_AUTO_TEST_CASE(emptyRulesGiveEmptyGraph)
{
  const GameRules rules;
  const RuleGraph graph(rules);
  BOOST_TEST(graph.vertexCount() == 0u);
  BOOST_TEST(graph.findCycle(nullptr) == 0u);
}

BOOST_AUTO_TEST_CASE(chainHasNoCycle)
{
  GameRules rules;
  rules.insert("Demon>Beast", true);
  rules.insert("Beast>Machine", true);
  const RuleGraph graph(rules);
  BOOST_TEST(graph.vertexCount() == 3u);

  size_t cycle[3] = {0, 0, 0};
  BOOST_TEST(graph.findCycle(cycle) == 0u);
}

BOOST_AUTO_TEST_CASE(topologicalOrderRespectsRules)
{
  GameRules rules;
  rules.insert("Demon>Beast", true);
  rules.insert("Beast>Machine", true);
  rules.insert("Demon>Machine", true);
  const RuleGraph graph(rules);

  size_t order[3] = {0, 0, 0};
  graph.topologicalOrder(order);
  const size_t demon = findPosition(graph, order, "Demon");
  const size_t beast = findPosition(graph, order, "Beast");
  const size_t machine = findPosition(graph, order, "Machine");
  BOOST_TEST(demon < beast);
  BOOST_TEST(beast < machine);
}

BOOST_AUTO_TEST_CASE(cycleIsFound)
{
  GameRules rules;
  rules.insert("Demon>Beast", true);
  rules.insert("Beast>Machine", true);
  rules.insert("Machine>Demon", true);
  const RuleGraph graph(rules);

  size_t cycle[3] = {0, 0, 0};
  BOOST_REQUIRE(graph.findCycle(cycle) == 3u);
  std::set< std::string > names;
  for (size_t i = 0; i < 3; ++i) {
    names.insert(graph.vertexName(cycle[i]));
  }
  BOOST_TEST(names.size() == 3u);
  BOOST_TEST(names.count("Demon") == 1u);
  BOOST_TEST(names.count("Beast") == 1u);
  BOOST_TEST(names.count("Machine") == 1u);
}

BOOST_AUTO_TEST_CASE(selfLoopIsFound)
{
  GameRules rules;
  rules.insert("Demon>Demon", true);
  const RuleGraph graph(rules);

  size_t cycle[1] = {0};
  BOOST_REQUIRE(graph.findCycle(cycle) == 1u);
  BOOST_TEST(graph.vertexName(cycle[0]) == "Demon");
}

BOOST_AUTO_TEST_CASE(reachableFollowsChains)
{
  GameRules rules;
  rules.insert("Demon>Beast", true);
  rules.insert("Beast>Machine", true);
  const RuleGraph graph(rules);

  size_t reachable[3] = {0, 0, 0};
  BOOST_REQUIRE(graph.reachableFrom("Demon", reachable) == 2u);
  std::set< std::string > names;
  names.insert(graph.vertexName(reachable[0]));
  names.insert(graph.vertexName(reachable[1]));
  BOOST_TEST(names.count("Beast") == 1u);
  BOOST_TEST(names.count("Machine") == 1u);

  BOOST_TEST(graph.reachableFrom("Machine", reachable) == 0u);
  BOOST_TEST(graph.reachableFrom("Unknown", reachable) == 0u);
}

BOOST_AUTO_TEST_CASE(reachableIncludesSelfInCycle)
{
  GameRules rules;
  rules.insert("Demon>Beast", true);
  rules.insert("Beast>Demon", true);
  const RuleGraph graph(rules);

  size_t reachable[2] = {0, 0};
  BOOST_REQUIRE(graph.reachableFrom("Demon", reachable) == 2u);
  std::set< std::string > names;
  names.insert(graph.vertexName(reachable[0]));
  names.insert(graph.vertexName(reachable[1]));
  BOOST_TEST(names.count("Demon") == 1u);
  BOOST_TEST(names.count("Beast") == 1u);
}

BOOST_AUTO_TEST_SUITE_END()
