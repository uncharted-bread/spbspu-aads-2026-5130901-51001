#include "commands.hpp"
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include "backpack.hpp"
#include "rulegraph.hpp"

static void fail(std::ostream& out)
{
  out << "<INVALID COMMAND>" << "\n";
}

static int parseInt(const std::string& str)
{
  size_t pos = 0;
  const int value = std::stoi(str, &pos);
  if (pos != str.size()) {
    throw std::invalid_argument("Not a whole number: " + str);
  }
  return value;
}

static void printSortedNames(const gordejchik::Deck& deck,
    std::string* names, size_t count, std::ostream& out)
{
  std::sort(names, names + count);
  for (size_t i = 0; i < count; ++i) {
    const gordejchik::Card& card = deck.at(names[i]);
    out << names[i] << ": POWER " << card.power
        << ", COST " << card.cost << "\n";
  }
}

static void printBackpackCards(const gordejchik::Deck& deck,
    const gordejchik::BackpackResult& res, std::ostream& out)
{
  const size_t count = res.count();
  if (count == 0) {
    return;
  }
  std::string* names = new std::string[count];
  try {
    for (size_t i = 0; i < count; ++i) {
      names[i] = res.cards()[i]->name;
    }
    printSortedNames(deck, names, count, out);
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

static gordejchik::BackpackResult solveDeck(
    gordejchik::Deck& deck, int budget)
{
  using gordejchik::Card;
  using gordejchik::Deck;
  using gordejchik::BackpackResult;
  const size_t count = deck.size();
  if (count == 0) {
    return BackpackResult(nullptr, 0, 0);
  }
  Card** cards = new Card*[count];
  size_t idx = 0;
  for (Deck::Iterator it = deck.begin(); it != deck.end(); ++it) {
    cards[idx] = &it->second;
    ++idx;
  }
  BackpackResult res(nullptr, 0, 0);
  try {
    res = gordejchik::solveBackpack(cards, count, budget);
  } catch (...) {
    delete[] cards;
    throw;
  }
  delete[] cards;
  return res;
}

static void printConfig(const gordejchik::game_config_t& config,
    std::ostream& out)
{
  using gordejchik::GameRules;
  out << "Type bonuses: " << (config.bonusEnabled ? "ON" : "OFF") << "\n";
  out << "Bonus value: " << config.bonusValue << "\n";
  out << "Rules:" << "\n";
  const size_t count = config.rules.size();
  if (count == 0) {
    return;
  }
  std::string* keys = new std::string[count];
  try {
    size_t idx = 0;
    for (GameRules::ConstIterator it = config.rules.cbegin(); it != config.rules.cend(); ++it) {
      keys[idx] = it->first;
      ++idx;
    }
    std::sort(keys, keys + count);
    for (size_t i = 0; i < count; ++i) {
      const size_t sep = keys[i].find('>');
      out << "  " << keys[i].substr(0, sep)
          << " > " << keys[i].substr(sep + 1) << "\n";
    }
  } catch (...) {
    delete[] keys;
    throw;
  }
  delete[] keys;
}

static void configBonus(gordejchik::game_config_t& config,
    const std::string& arg, std::ostream& out)
{
  if (arg == "on") {
    config.bonusEnabled = true;
    std::cerr << "Type bonuses enabled" << "\n";
    return;
  }
  if (arg == "off") {
    config.bonusEnabled = false;
    std::cerr << "Type bonuses disabled" << "\n";
    return;
  }
  int value = 0;
  try {
    value = parseInt(arg);
  } catch (const std::exception&) {
    fail(out);
    return;
  }
  if (value < 0) {
    fail(out);
    return;
  }
  config.bonusValue = value;
  std::cerr << "Bonus value set to " << value << "\n";
}

static void configRule(gordejchik::game_config_t& config,
    const std::string& typeA, const std::string& typeB, std::ostream& out)
{
  if (typeA.empty() || typeB.empty() || typeA == typeB) {
    fail(out);
    return;
  }
  config.rules.insert(typeA + ">" + typeB, true);
  std::cerr << "Rule '" << typeA << " > " << typeB << "' set" << "\n";
}

static void configAnalyze(const gordejchik::game_config_t& config, std::ostream& out)
{
  const gordejchik::RuleGraph graph(config.rules);
  const size_t count = graph.vertexCount();
  if (count == 0) {
    out << "No rules to analyze" << "\n";
    return;
  }
  size_t* buffer = new size_t[count];
  try {
    const size_t cycleLength = graph.findCycle(buffer);
    if (cycleLength > 0) {
      out << "Cycle: ";
      for (size_t i = 0; i < cycleLength; ++i) {
        out << graph.vertexName(buffer[i]) << " > ";
      }
      out << graph.vertexName(buffer[0]) << "\n";
    } else {
      graph.topologicalOrder(buffer);
      out << "Type ranking:" << "\n";
      for (size_t i = 0; i < count; ++i) {
        out << "  " << (i + 1) << ". " << graph.vertexName(buffer[i]) << "\n";
      }
    }
  } catch (...) {
    delete[] buffer;
    throw;
  }
  delete[] buffer;
}

static void configDominates(const gordejchik::game_config_t& config,
    const std::string& type, std::ostream& out)
{
  const gordejchik::RuleGraph graph(config.rules);
  const size_t count = graph.vertexCount();
  size_t found = 0;
  size_t* indices = nullptr;
  std::string* names = nullptr;
  try {
    if (count > 0) {
      indices = new size_t[count];
      found = graph.reachableFrom(type, indices);
    }
    if (found == 0) {
      out << type << " dominates nothing" << "\n";
    } else {
      names = new std::string[found];
      for (size_t i = 0; i < found; ++i) {
        names[i] = graph.vertexName(indices[i]);
      }
      std::sort(names, names + found);
      out << type << " dominates:";
      for (size_t i = 0; i < found; ++i) {
        out << " " << names[i];
      }
      out << "\n";
    }
  } catch (...) {
    delete[] indices;
    delete[] names;
    throw;
  }
  delete[] indices;
  delete[] names;
}

static void printBattleSide(const std::string& deckName,
    const gordejchik::Deck& deck,
    const gordejchik::BackpackResult& res, int bonus, bool showBonus,
    std::ostream& out)
{
  out << "=== " << deckName << " ===" << "\n";
  printBackpackCards(deck, res, out);
  if (showBonus) {
    out << "BASE POWER: " << res.totalPower() << "\n";
    out << "TYPE BONUS: +" << bonus << "\n";
  }
  out << "TOTAL POWER: " << (res.totalPower() + bonus) << "\n";
}

static void writeDeck(std::ostream& file, const gordejchik::Deck& deck)
{
  using gordejchik::Card;
  using gordejchik::Deck;
  file << deck.size() << "\n";
  for (Deck::ConstIterator it = deck.cbegin(); it != deck.cend(); ++it) {
    const Card& card = it->second;
    file << card.name << "\n";
    file << card.power << " " << card.cost << "\n";
    file << card.type << "\n";
    file << card.description << "\n";
  }
}

static void readDeckCards(std::istream& file, gordejchik::Deck& deck)
{
  using gordejchik::Card;
  size_t count = 0;
  if (!(file >> count)) {
    throw std::invalid_argument("Bad card count");
  }
  file.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
  for (size_t i = 0; i < count; ++i) {
    std::string cardName;
    std::string stats;
    if (!std::getline(file, cardName) || !std::getline(file, stats)) {
      throw std::invalid_argument("Truncated card record");
    }
    const size_t space = stats.find(' ');
    if (space == std::string::npos) {
      throw std::invalid_argument("Bad card stats");
    }
    const int power = parseInt(stats.substr(0, space));
    const int cost = parseInt(stats.substr(space + 1));
    if (cost < 0) {
      throw std::invalid_argument("Negative card cost");
    }
    std::string type;
    std::string desc;
    if (!std::getline(file, type) || !std::getline(file, desc)) {
      throw std::invalid_argument("Truncated card record");
    }
    deck.insert(cardName, Card{cardName, power, cost, type, desc});
  }
}

gordejchik::ParsedCommand gordejchik::parseLine(
    const std::string& line)
{
  ParsedCommand result;
  result.count = 0;
  size_t i = 0;
  const size_t len = line.size();
  while (i < len && result.count < MAX_TOKENS) {
    while (i < len && line[i] == ' ') {
      ++i;
    }
    if (i >= len) {
      break;
    }
    if (line[i] == '"') {
      ++i;
      std::string token;
      while (i < len && line[i] != '"') {
        token += line[i];
        ++i;
      }
      if (i < len) {
        ++i;
      }
      result.tokens[result.count] = token;
      ++result.count;
    } else {
      std::string token;
      while (i < len && line[i] != ' ') {
        token += line[i];
        ++i;
      }
      result.tokens[result.count] = token;
      ++result.count;
    }
  }
  return result;
}

void gordejchik::cmdCreate(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 2) {
    fail(out);
    return;
  }
  const std::string& name = cmd.tokens[1];
  if (decks.contains(name)) {
    fail(out);
    return;
  }
  decks.insert(name, Deck());
  std::cerr << "Deck '" << name << "' created" << "\n";
}

void gordejchik::cmdDelete(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 2) {
    fail(out);
    return;
  }
  const std::string& name = cmd.tokens[1];
  if (!decks.contains(name)) {
    fail(out);
    return;
  }
  decks.erase(name);
  std::cerr << "Deck '" << name << "' deleted" << "\n";
}

void gordejchik::cmdHelp(DeckStore&, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 1) {
    fail(out);
    return;
  }
  out << "create <deck>                             "
      << "- create an empty deck" << "\n";
  out << "delete <deck>                             "
      << "- delete a deck" << "\n";
  out << "add <deck> <card> <power> <cost>          "
      << "- add a card" << "\n";
  out << "remove <deck> <card>                      "
      << "- remove a card" << "\n";
  out << "set-type <deck> <card> <type>             "
      << "- set card type" << "\n";
  out << "set-desc <deck> <card> <text>             "
      << "- set card description" << "\n";
  out << "show <deck>                               "
      << "- show all cards" << "\n";
  out << "info <deck> <card>                        "
      << "- full card info" << "\n";
  out << "range <deck> <stat> <min> <max>           "
      << "- filter by stat range" << "\n";
  out << "merge <new-deck> <deck-1> <deck-2>        "
      << "- merge two decks" << "\n";
  out << "optimize <deck> <budget> [<new-deck>]     "
      << "- pick cards (knapsack)" << "\n";
  out << "battle <deck-1> <deck-2> <budget>         "
      << "- compare two decks with type bonuses" << "\n";
  out << "save <deck> <filename>                    "
      << "- save deck to file" << "\n";
  out << "load <deck> <filename>                    "
      << "- load deck from file" << "\n";
  out << "trade <deck-1> <deck-2> <card-1> <card-2> "
      << "- swap two cards between decks" << "\n";
  out << "config                                    "
      << "- show type bonus settings" << "\n";
  out << "config bonus <value>|on|off               "
      << "- set bonus value or toggle bonuses" << "\n";
  out << "config rule <type-a> <type-b>             "
      << "- make type-a dominate type-b" << "\n";
  out << "config clear-rules                        "
      << "- delete all dominance rules" << "\n";
  out << "config analyze                            "
      << "- find rule cycles or rank types" << "\n";
  out << "config dominates <type>                   "
      << "- list types beaten via rule chains" << "\n";
  out << "help                                      "
      << "- list commands" << "\n";
}

void gordejchik::cmdAdd(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 5) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  DeckStore::Iterator deckIt = decks.find(deckName);
  if (deckIt == decks.end()) {
    fail(out);
    return;
  }
  int power = 0;
  int cost = 0;
  try {
    power = parseInt(cmd.tokens[3]);
    cost = parseInt(cmd.tokens[4]);
  } catch (const std::exception&) {
    fail(out);
    return;
  }
  if (cost < 0) {
    fail(out);
    return;
  }
  Deck& deck = deckIt->second;
  if (deck.contains(cardName)) {
    fail(out);
    return;
  }
  deck.insert(cardName, Card{cardName, power, cost, "", ""});
  std::cerr << "Card '" << cardName << "' added to '" << deckName << "'" << "\n";
}

void gordejchik::cmdRemove(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 3) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  DeckStore::Iterator deckIt = decks.find(deckName);
  if (deckIt == decks.end()) {
    fail(out);
    return;
  }
  Deck& deck = deckIt->second;
  if (!deck.contains(cardName)) {
    fail(out);
    return;
  }
  deck.erase(cardName);
  std::cerr << "Card '" << cardName << "' removed from '" << deckName << "'" << "\n";
}

void gordejchik::cmdShow(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 2) {
    fail(out);
    return;
  }
  const std::string& name = cmd.tokens[1];
  DeckStore::Iterator deckIt = decks.find(name);
  if (deckIt == decks.end()) {
    fail(out);
    return;
  }
  const Deck& deck = deckIt->second;
  const size_t count = deck.size();
  std::cerr << "Deck '" << name << "': " << count << " cards" << "\n";
  if (count == 0) {
    return;
  }
  std::string* names = new std::string[count];
  try {
    size_t idx = 0;
    for (Deck::ConstIterator it = deck.cbegin(); it != deck.cend(); ++it) {
      names[idx] = it->first;
      ++idx;
    }
    printSortedNames(deck, names, count, out);
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

void gordejchik::cmdInfo(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 3) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  DeckStore::Iterator deckIt = decks.find(deckName);
  if (deckIt == decks.end()) {
    fail(out);
    return;
  }
  Deck& deck = deckIt->second;
  Deck::Iterator cardIt = deck.find(cardName);
  if (cardIt == deck.end()) {
    fail(out);
    return;
  }
  const Card& card = cardIt->second;
  std::string upper = card.name;
  for (size_t i = 0; i < upper.size(); ++i) {
    if (upper[i] >= 'a' && upper[i] <= 'z') {
      upper[i] = static_cast< char >(upper[i] - 'a' + 'A');
    }
  }
  std::string header = "=== " + upper + " ===";
  out << header << "\n";
  if (card.type.empty()) {
    out << "TYPE: <none>" << "\n";
  } else {
    out << "TYPE: " << card.type << "\n";
  }
  out << "POWER: " << card.power
      << ", COST: " << card.cost << "\n";
  out << "DATA:" << "\n";
  if (card.description.empty()) {
    out << "<no data>" << "\n";
  } else {
    out << card.description << "\n";
  }
  std::string footer(header.size(), '=');
  out << footer << "\n";
}

void gordejchik::cmdSetType(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  DeckStore::Iterator deckIt = decks.find(deckName);
  if (deckIt == decks.end()) {
    fail(out);
    return;
  }
  Deck::Iterator cardIt = deckIt->second.find(cardName);
  if (cardIt == deckIt->second.end()) {
    fail(out);
    return;
  }
  cardIt->second.type = cmd.tokens[3];
  std::cerr << "Type of card '" << cardName << "' set" << "\n";
}

void gordejchik::cmdSetDesc(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  DeckStore::Iterator deckIt = decks.find(deckName);
  if (deckIt == decks.end()) {
    fail(out);
    return;
  }
  Deck::Iterator cardIt = deckIt->second.find(cardName);
  if (cardIt == deckIt->second.end()) {
    fail(out);
    return;
  }
  cardIt->second.description = cmd.tokens[3];
  std::cerr << "Description of card '" << cardName << "' set" << "\n";
}

void gordejchik::cmdRange(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 5) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& stat = cmd.tokens[2];
  DeckStore::Iterator deckIt = decks.find(deckName);
  if (deckIt == decks.end()) {
    fail(out);
    return;
  }
  if (stat != "power" && stat != "cost") {
    fail(out);
    return;
  }
  int minVal = 0;
  int maxVal = 0;
  try {
    minVal = parseInt(cmd.tokens[3]);
    maxVal = parseInt(cmd.tokens[4]);
  } catch (const std::exception&) {
    fail(out);
    return;
  }
  if (minVal > maxVal) {
    fail(out);
    return;
  }
  const Deck& deck = deckIt->second;
  const size_t total = deck.size();
  if (total == 0) {
    std::cerr << "Cards found: 0" << "\n";
    return;
  }
  std::string* names = new std::string[total];
  try {
    size_t found = 0;
    const bool usePower = (stat == "power");
    for (Deck::ConstIterator it = deck.cbegin(); it != deck.cend(); ++it) {
      const Card& card = it->second;
      const int val = usePower ? card.power : card.cost;
      if (val >= minVal && val <= maxVal) {
        names[found] = it->first;
        ++found;
      }
    }
    printSortedNames(deck, names, found, out);
    std::cerr << "Cards found: " << found << "\n";
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

void gordejchik::cmdOptimize(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 3 && cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string newDeckName = (cmd.count == 4) ? cmd.tokens[3] : "";
  DeckStore::Iterator deckIt = decks.find(deckName);
  if (deckIt == decks.end()) {
    fail(out);
    return;
  }
  if (!newDeckName.empty() && decks.contains(newDeckName)) {
    fail(out);
    return;
  }
  int budget = 0;
  try {
    budget = parseInt(cmd.tokens[2]);
  } catch (const std::exception&) {
    fail(out);
    return;
  }
  if (budget < 0) {
    fail(out);
    return;
  }
  Deck& deck = deckIt->second;
  BackpackResult res = solveDeck(deck, budget);
  printBackpackCards(deck, res, out);
  out << "TOTAL POWER: " << res.totalPower() << "\n";
  if (newDeckName.empty()) {
    return;
  }
  Deck newDeck;
  for (size_t i = 0; i < res.count(); ++i) {
    const Card* card = res.cards()[i];
    newDeck.insert(card->name, *card);
  }
  decks.insert(newDeckName, newDeck);
  std::cerr << "Created deck '" << newDeckName << "'" << "\n";
}

void gordejchik::cmdBattle(DeckStore& decks, game_config_t& config,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& deck1Name = cmd.tokens[1];
  const std::string& deck2Name = cmd.tokens[2];
  DeckStore::Iterator it1 = decks.find(deck1Name);
  DeckStore::Iterator it2 = decks.find(deck2Name);
  if (it1 == decks.end() || it2 == decks.end()) {
    fail(out);
    return;
  }
  int budget = 0;
  try {
    budget = parseInt(cmd.tokens[3]);
  } catch (const std::exception&) {
    fail(out);
    return;
  }
  if (budget < 0) {
    fail(out);
    return;
  }
  Deck& deck1 = it1->second;
  Deck& deck2 = it2->second;
  BackpackResult res1 = solveDeck(deck1, budget);
  BackpackResult res2 = solveDeck(deck2, budget);
  const std::pair< int, int > bonuses = computeTypeBonuses(res1, res2, config);
  printBattleSide(deck1Name, deck1, res1, bonuses.first, config.bonusEnabled, out);
  printBattleSide(deck2Name, deck2, res2, bonuses.second, config.bonusEnabled, out);
  const int total1 = res1.totalPower() + bonuses.first;
  const int total2 = res2.totalPower() + bonuses.second;
  if (total1 > total2) {
    out << "WINNER: " << deck1Name << "\n";
  } else if (total2 > total1) {
    out << "WINNER: " << deck2Name << "\n";
  } else {
    out << "DRAW" << "\n";
  }
}

void gordejchik::cmdMerge(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& newName = cmd.tokens[1];
  DeckStore::Iterator it1 = decks.find(cmd.tokens[2]);
  DeckStore::Iterator it2 = decks.find(cmd.tokens[3]);
  if (it1 == decks.end() || it2 == decks.end()) {
    fail(out);
    return;
  }
  if (decks.contains(newName)) {
    fail(out);
    return;
  }
  const Deck& d1 = it1->second;
  const Deck& d2 = it2->second;
  Deck merged;
  for (Deck::ConstIterator it = d1.cbegin(); it != d1.cend(); ++it) {
    merged.insert(it->first, it->second);
  }
  for (Deck::ConstIterator it = d2.cbegin(); it != d2.cend(); ++it) {
    if (!merged.contains(it->first)) {
      merged.insert(it->first, it->second);
    }
  }
  decks.insert(newName, merged);
  std::cerr << "Decks merged into '" << newName << "'" << "\n";
}

void gordejchik::cmdSave(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 3) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& filename = cmd.tokens[2];
  DeckStore::Iterator deckIt = decks.find(deckName);
  if (deckIt == decks.end()) {
    fail(out);
    return;
  }
  const Deck& deck = deckIt->second;
  std::ofstream file(filename);
  if (!file.is_open()) {
    fail(out);
    return;
  }
  writeDeck(file, deck);
  std::cerr << "Deck saved to '" << filename << "'" << "\n";
}

void gordejchik::cmdLoad(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 3) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& filename = cmd.tokens[2];
  if (decks.contains(deckName)) {
    fail(out);
    return;
  }
  std::ifstream file(filename);
  if (!file.is_open()) {
    fail(out);
    return;
  }
  Deck deck;
  try {
    readDeckCards(file, deck);
  } catch (const std::exception&) {
    fail(out);
    return;
  }
  decks.insert(deckName, deck);
  std::cerr << "Deck loaded from '" << filename << "'" << "\n";
}

void gordejchik::cmdTrade(DeckStore& decks, game_config_t&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 5) {
    fail(out);
    return;
  }
  const std::string& deck1Name = cmd.tokens[1];
  const std::string& deck2Name = cmd.tokens[2];
  const std::string& card1Name = cmd.tokens[3];
  const std::string& card2Name = cmd.tokens[4];
  DeckStore::Iterator it1 = decks.find(deck1Name);
  DeckStore::Iterator it2 = decks.find(deck2Name);
  if (it1 == decks.end() || it2 == decks.end()) {
    fail(out);
    return;
  }
  Deck& deck1 = it1->second;
  Deck& deck2 = it2->second;
  if (!deck1.contains(card1Name) || !deck2.contains(card2Name)) {
    fail(out);
    return;
  }
  const bool sameDeck = (deck1Name == deck2Name);
  if (sameDeck && card1Name == card2Name) {
    fail(out);
    return;
  }
  if (!sameDeck && (deck2.contains(card1Name) || deck1.contains(card2Name))) {
    fail(out);
    return;
  }
  const Card card1 = deck1.at(card1Name);
  const Card card2 = deck2.at(card2Name);
  deck1.erase(card1Name);
  deck2.erase(card2Name);
  try {
    deck2.insert(card1Name, card1);
    deck1.insert(card2Name, card2);
  } catch (...) {
    if (deck2.contains(card1Name)) {
      deck2.erase(card1Name);
    }
    deck1.insert(card1Name, card1);
    deck2.insert(card2Name, card2);
    throw;
  }
  std::cerr << "Card '" << card1Name << "' traded for '" << card2Name << "'" << "\n";
}

void gordejchik::cmdConfig(DeckStore&, game_config_t& config,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count == 1) {
    printConfig(config, out);
    return;
  }
  const std::string& sub = cmd.tokens[1];
  if (sub == "bonus" && cmd.count == 3) {
    configBonus(config, cmd.tokens[2], out);
    return;
  }
  if (sub == "rule" && cmd.count == 4) {
    configRule(config, cmd.tokens[2], cmd.tokens[3], out);
    return;
  }
  if (sub == "clear-rules" && cmd.count == 2) {
    config.rules = GameRules();
    std::cerr << "Rules cleared" << "\n";
    return;
  }
  if (sub == "analyze" && cmd.count == 2) {
    configAnalyze(config, out);
    return;
  }
  if (sub == "dominates" && cmd.count == 3) {
    configDominates(config, cmd.tokens[2], out);
    return;
  }
  fail(out);
}

gordejchik::HashTable< std::string, gordejchik::CommandHandler >
gordejchik::makeCommandTable()
{
  HashTable< std::string, CommandHandler > table;
  table.insert("help", cmdHelp);
  table.insert("create", cmdCreate);
  table.insert("delete", cmdDelete);
  table.insert("show", cmdShow);
  table.insert("add", cmdAdd);
  table.insert("remove", cmdRemove);
  table.insert("info", cmdInfo);
  table.insert("set-type", cmdSetType);
  table.insert("set-desc", cmdSetDesc);
  table.insert("range", cmdRange);
  table.insert("optimize", cmdOptimize);
  table.insert("battle", cmdBattle);
  table.insert("merge", cmdMerge);
  table.insert("save", cmdSave);
  table.insert("load", cmdLoad);
  table.insert("trade", cmdTrade);
  table.insert("config", cmdConfig);
  return table;
}
