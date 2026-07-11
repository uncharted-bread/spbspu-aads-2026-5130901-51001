#include "commands.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "backpack.hpp"

static void fail(std::ostream& out)
{
  out << "<INVALID COMMAND>" << "\n";
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

void gordejchik::cmdCreate(DeckStore& decks,
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

void gordejchik::cmdDelete(DeckStore& decks,
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

void gordejchik::cmdHelp(DeckStore&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 1) {
    fail(out);
    return;
  }
  out << "create <deck>                        "
      << "- create an empty deck" << "\n";
  out << "delete <deck>                        "
      << "- delete a deck" << "\n";
  out << "add <deck> <card> <power> <cost>     "
      << "- add a card" << "\n";
  out << "remove <deck> <card>                 "
      << "- remove a card" << "\n";
  out << "set-type <deck> <card> <type>        "
      << "- set card type" << "\n";
  out << "set-desc <deck> <card> <text>        "
      << "- set card description" << "\n";
  out << "show <deck>                          "
      << "- show all cards" << "\n";
  out << "info <deck> <card>                   "
      << "- full card info" << "\n";
  out << "range <deck> <stat> <min> <max>      "
      << "- filter by stat range" << "\n";
  out << "merge <new-deck> <deck-1> <deck-2>   "
      << "- merge two decks" << "\n";
  out << "optimize <deck> <budget> [<new-deck>]"
      << " - pick cards (knapsack)" << "\n";
  out << "battle <deck-1> <deck-2> <budget>    "
      << "- compare two decks" << "\n";
  out << "save <deck> <filename>               "
      << "- save deck to file" << "\n";
  out << "load <deck> <filename>               "
      << "- load deck from file" << "\n";
  out << "help                                 "
      << "- list commands" << "\n";
}

void gordejchik::cmdAdd(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 5) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  const std::string& powerStr = cmd.tokens[3];
  const std::string& costStr = cmd.tokens[4];
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  int power = 0;
  int cost = 0;
  try {
    size_t pos = 0;
    power = std::stoi(powerStr, &pos);
    if (pos != powerStr.size()) {
      fail(out);
      return;
    }
    pos = 0;
    cost = std::stoi(costStr, &pos);
    if (pos != costStr.size()) {
      fail(out);
      return;
    }
  } catch (const std::invalid_argument&) {
    fail(out);
    return;
  } catch (const std::out_of_range&) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  if (deck.contains(cardName)) {
    fail(out);
    return;
  }
  deck.insert(cardName, Card{cardName, power, cost, "", ""});
  std::cerr << "Card '" << cardName << "' added to '" << deckName << "'" << "\n";
}

void gordejchik::cmdRemove(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 3) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  if (!deck.contains(cardName)) {
    fail(out);
    return;
  }
  deck.erase(cardName);
  std::cerr << "Card '" << cardName << "' removed from '" << deckName << "'" << "\n";
}

void gordejchik::cmdShow(DeckStore& decks,
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
  const Deck& deck = decks.at(name);
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
    std::sort(names, names + count);
    for (size_t i = 0; i < count; ++i) {
      const Card& card = deck.at(names[i]);
      out << names[i] << ": POWER " << card.power
          << ", COST " << card.cost << "\n";
    }
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

void gordejchik::cmdInfo(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 3) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  if (!deck.contains(cardName)) {
    fail(out);
    return;
  }
  const Card& card = deck.at(cardName);
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

void gordejchik::cmdSetType(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  const std::string& type = cmd.tokens[3];
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  if (!deck.contains(cardName)) {
    fail(out);
    return;
  }
  deck.at(cardName).type = type;
  std::cerr << "Type of card '" << cardName << "' set" << "\n";
}

void gordejchik::cmdSetDesc(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& cardName = cmd.tokens[2];
  const std::string& desc = cmd.tokens[3];
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  if (!deck.contains(cardName)) {
    fail(out);
    return;
  }
  deck.at(cardName).description = desc;
  std::cerr << "Description of card '" << cardName << "' set" << "\n";
}

void gordejchik::cmdRange(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 5) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& stat = cmd.tokens[2];
  const std::string& minStr = cmd.tokens[3];
  const std::string& maxStr = cmd.tokens[4];
  if (!decks.contains(deckName)) {
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
    size_t pos = 0;
    minVal = std::stoi(minStr, &pos);
    if (pos != minStr.size()) {
      fail(out);
      return;
    }
    pos = 0;
    maxVal = std::stoi(maxStr, &pos);
    if (pos != maxStr.size()) {
      fail(out);
      return;
    }
  } catch (const std::invalid_argument&) {
    fail(out);
    return;
  } catch (const std::out_of_range&) {
    fail(out);
    return;
  }
  if (minVal > maxVal) {
    fail(out);
    return;
  }
  const Deck& deck = decks.at(deckName);
  const size_t total = deck.size();
  if (total == 0) {
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
    std::sort(names, names + found);
    for (size_t i = 0; i < found; ++i) {
      const Card& card = deck.at(names[i]);
      out << names[i] << ": POWER " << card.power
          << ", COST " << card.cost << "\n";
    }
    std::cerr << "Cards found: " << found << "\n";
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

void gordejchik::cmdOptimize(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 3 && cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& budgetStr = cmd.tokens[2];
  const std::string newDeckName = (cmd.count == 4) ? cmd.tokens[3] : "";
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  if (!newDeckName.empty() && decks.contains(newDeckName)) {
    fail(out);
    return;
  }
  int budget = 0;
  try {
    size_t pos = 0;
    budget = std::stoi(budgetStr, &pos);
    if (pos != budgetStr.size()) {
      fail(out);
      return;
    }
  } catch (const std::invalid_argument&) {
    fail(out);
    return;
  } catch (const std::out_of_range&) {
    fail(out);
    return;
  }
  if (budget < 0) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  const size_t count = deck.size();
  if (count == 0) {
    out << "TOTAL POWER: 0" << "\n";
    return;
  }
  Card** allCards = new Card*[count];
  size_t idx = 0;
  for (Deck::Iterator it = deck.begin(); it != deck.end(); ++it) {
    allCards[idx] = &it->second;
    ++idx;
  }
  BackpackResult res(nullptr, 0, 0);
  try {
    res = solveBackpack(allCards, count, budget);
  } catch (...) {
    delete[] allCards;
    throw;
  }
  delete[] allCards;
  std::string* names = new std::string[res.count()];
  try {
    for (size_t i = 0; i < res.count(); ++i) {
      names[i] = res.cards()[i]->name;
    }
    std::sort(names, names + res.count());
    for (size_t i = 0; i < res.count(); ++i) {
      const Card& card = deck.at(names[i]);
      out << names[i] << ": POWER " << card.power
          << ", COST " << card.cost << "\n";
    }
    out << "TOTAL POWER: " << res.totalPower() << "\n";
    if (!newDeckName.empty()) {
      Deck newDeck;
      for (size_t i = 0; i < res.count(); ++i) {
        newDeck.insert(names[i], deck.at(names[i]));
      }
      decks.insert(newDeckName, newDeck);
      std::cerr << "Created deck '" << newDeckName << "'" << "\n";
    }
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

void gordejchik::cmdBattle(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& deck1Name = cmd.tokens[1];
  const std::string& deck2Name = cmd.tokens[2];
  const std::string& budgetStr = cmd.tokens[3];
  if (!decks.contains(deck1Name)
      || !decks.contains(deck2Name)) {
    fail(out);
    return;
  }
  int budget = 0;
  try {
    size_t pos = 0;
    budget = std::stoi(budgetStr, &pos);
    if (pos != budgetStr.size()) {
      fail(out);
      return;
    }
  } catch (const std::invalid_argument&) {
    fail(out);
    return;
  } catch (const std::out_of_range&) {
    fail(out);
    return;
  }
  if (budget < 0) {
    fail(out);
    return;
  }
  Deck& deck1 = decks.at(deck1Name);
  Deck& deck2 = decks.at(deck2Name);
  const size_t count1 = deck1.size();
  const size_t count2 = deck2.size();
  Card** arr1 = nullptr;
  Card** arr2 = nullptr;
  BackpackResult res1(nullptr, 0, 0);
  BackpackResult res2(nullptr, 0, 0);
  try {
    if (count1 > 0) {
      arr1 = new Card*[count1];
      size_t idx = 0;
      for (Deck::Iterator it = deck1.begin(); it != deck1.end(); ++it) {
        arr1[idx] = &it->second;
        ++idx;
      }
    }
    if (count2 > 0) {
      arr2 = new Card*[count2];
      size_t idx = 0;
      for (Deck::Iterator it = deck2.begin(); it != deck2.end(); ++it) {
        arr2[idx] = &it->second;
        ++idx;
      }
    }
    res1 = solveBackpack(arr1, count1, budget);
    res2 = solveBackpack(arr2, count2, budget);
  } catch (...) {
    delete[] arr1;
    delete[] arr2;
    throw;
  }
  delete[] arr1;
  delete[] arr2;
  out << "=== " << deck1Name << " ===" << "\n";
  if (res1.count() > 0) {
    std::string* names1 = new std::string[res1.count()];
    try {
      for (size_t i = 0; i < res1.count(); ++i) {
        names1[i] = res1.cards()[i]->name;
      }
      std::sort(names1, names1 + res1.count());
      for (size_t i = 0; i < res1.count(); ++i) {
        const Card& card = deck1.at(names1[i]);
        out << names1[i] << ": POWER " << card.power
            << ", COST " << card.cost << "\n";
      }
    } catch (...) {
      delete[] names1;
      throw;
    }
    delete[] names1;
  }
  out << "TOTAL POWER: " << res1.totalPower() << "\n";
  out << "=== " << deck2Name << " ===" << "\n";
  if (res2.count() > 0) {
    std::string* names2 = new std::string[res2.count()];
    try {
      for (size_t i = 0; i < res2.count(); ++i) {
        names2[i] = res2.cards()[i]->name;
      }
      std::sort(names2, names2 + res2.count());
      for (size_t i = 0; i < res2.count(); ++i) {
        const Card& card = deck2.at(names2[i]);
        out << names2[i] << ": POWER " << card.power
            << ", COST " << card.cost << "\n";
      }
    } catch (...) {
      delete[] names2;
      throw;
    }
    delete[] names2;
  }
  out << "TOTAL POWER: " << res2.totalPower() << "\n";
  if (res1.totalPower() > res2.totalPower()) {
    out << "WINNER: " << deck1Name << "\n";
  } else if (res2.totalPower() > res1.totalPower()) {
    out << "WINNER: " << deck2Name << "\n";
  } else {
    out << "DRAW" << "\n";
  }
}

void gordejchik::cmdMerge(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 4) {
    fail(out);
    return;
  }
  const std::string& newName = cmd.tokens[1];
  const std::string& name1 = cmd.tokens[2];
  const std::string& name2 = cmd.tokens[3];
  if (!decks.contains(name1) || !decks.contains(name2)) {
    fail(out);
    return;
  }
  if (decks.contains(newName)) {
    fail(out);
    return;
  }
  const Deck& d1 = decks.at(name1);
  const Deck& d2 = decks.at(name2);
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

void gordejchik::cmdSave(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count != 3) {
    fail(out);
    return;
  }
  const std::string& deckName = cmd.tokens[1];
  const std::string& filename = cmd.tokens[2];
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  std::ofstream file(filename);
  if (!file.is_open()) {
    fail(out);
    return;
  }
  const size_t count = deck.size();
  file << count << "\n";
  for (Deck::ConstIterator it = deck.cbegin(); it != deck.cend(); ++it) {
    const Card& card = it->second;
    file << card.name << " "
        << card.power << " "
        << card.cost << "\n";
    file << card.type << "\n";
    file << card.description << "\n";
  }
  std::cerr << "Deck saved to '" << filename << "'" << "\n";
}

void gordejchik::cmdLoad(DeckStore& decks,
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
  size_t count = 0;
  if (!(file >> count)) {
    fail(out);
    return;
  }
  file.ignore();
  Deck deck;
  for (size_t i = 0; i < count; ++i) {
    std::string header;
    if (!std::getline(file, header)) {
      fail(out);
      return;
    }
    size_t space1 = header.find(' ');
    if (space1 == std::string::npos) {
      fail(out);
      return;
    }
    size_t space2 = header.find(' ', space1 + 1);
    if (space2 == std::string::npos) {
      fail(out);
      return;
    }
    std::string cardName = header.substr(0, space1);
    std::string powerStr = header.substr(space1 + 1,
        space2 - space1 - 1);
    std::string costStr = header.substr(space2 + 1);
    int power = 0;
    int cost = 0;
    try {
      size_t pos = 0;
      power = std::stoi(powerStr, &pos);
      if (pos != powerStr.size()) {
        fail(out);
        return;
      }
      pos = 0;
      cost = std::stoi(costStr, &pos);
      if (pos != costStr.size()) {
        fail(out);
        return;
      }
    } catch (const std::invalid_argument&) {
      fail(out);
      return;
    } catch (const std::out_of_range&) {
      fail(out);
      return;
    }
    std::string type;
    std::string desc;
    if (!std::getline(file, type)
        || !std::getline(file, desc)) {
      fail(out);
      return;
    }
    deck.insert(cardName, Card{cardName, power, cost, type, desc});
  }
  decks.insert(deckName, deck);
  std::cerr << "Deck loaded from '" << filename << "'" << "\n";
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
  return table;
}
