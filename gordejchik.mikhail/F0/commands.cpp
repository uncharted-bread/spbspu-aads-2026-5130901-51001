#include "commands.hpp"
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include "backpack.hpp"

static void fail(std::ostream& out)
{
  out << "<INVALID COMMAND>" << "\n";
}

void gordejchik::cmdCreate(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count_ != 2) {
    fail(out);
    return;
  }
  const std::string& name = cmd.tokens_[1];
  if (decks.contains(name)) {
    fail(out);
    return;
  }
  decks.insert(name, Deck{name, {}});
}

void gordejchik::cmdDelete(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count_ != 2) {
    fail(out);
    return;
  }
  const std::string& name = cmd.tokens_[1];
  if (!decks.contains(name)) {
    fail(out);
    return;
  }
  decks.erase(name);
}

void gordejchik::cmdHelp(DeckStore&,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count_ != 1) {
    fail(out);
    return;
  }
  out << "create <deck>                        "
      << "- создать пустую колоду" << "\n";
  out << "delete <deck>                        "
      << "- удалить колоду" << "\n";
  out << "add <deck> <card> <power> <cost>     "
      << "- добавить карту" << "\n";
  out << "remove <deck> <card>                 "
      << "- удалить карту" << "\n";
  out << "set-type <deck> <card> <type>        "
      << "- задать тип карты" << "\n";
  out << "set-desc <deck> <card> <text>        "
      << "- задать описание карты" << "\n";
  out << "show <deck>                          "
      << "- показать все карты" << "\n";
  out << "info <deck> <card>                   "
      << "- полная информация о карте" << "\n";
  out << "range <deck> <stat> <min> <max>      "
      << "- фильтр по диапазону" << "\n";
  out << "merge <new-deck> <deck-1> <deck-2>   "
      << "- объединить две колоды" << "\n";
  out << "optimize <deck> <budget> [<new-deck>]"
      << " - подбор карт (рюкзак)" << "\n";
  out << "battle <deck-1> <deck-2> <budget>    "
      << "- сравнить две колоды" << "\n";
  out << "save <deck> <filename>               "
      << "- сохранить в файл" << "\n";
  out << "load <deck> <filename>               "
      << "- загрузить из файла" << "\n";
  out << "help                                 "
      << "- список команд" << "\n";
}

void gordejchik::cmdAdd(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName,
    const std::string& powerStr,
    const std::string& costStr, std::ostream& out)
{
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
  if (deck.cards.contains(cardName)) {
    fail(out);
    return;
  }
  deck.cards.insert(cardName, Card{cardName, power, cost, "", ""});
}

void gordejchik::cmdRemove(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  if (!deck.cards.contains(cardName)) {
    fail(out);
    return;
  }
  deck.cards.erase(cardName);
}

void gordejchik::cmdShow(DeckStore& decks,
    const ParsedCommand& cmd, std::ostream& out)
{
  if (cmd.count_ != 2) {
    fail(out);
    return;
  }
  const std::string& name = cmd.tokens_[1];
  if (!decks.contains(name)) {
    fail(out);
    return;
  }
  const Deck& deck = decks.at(name);
  const size_t count = deck.cards.size();
  if (count == 0) {
    return;
  }
  std::string* names = new std::string[count];
  try {
    size_t idx = 0;
    using CardCIter = HashTable< std::string, Card >::ConstIterator;
    for (CardCIter it = deck.cards.cbegin(); it != deck.cards.cend(); ++it) {
      names[idx] = it->first;
      ++idx;
    }
    std::sort(names, names + count);
    for (size_t i = 0; i < count; ++i) {
      const Card& card = deck.cards.at(names[i]);
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
    const std::string& deckName,
    const std::string& cardName, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  if (!deck.cards.contains(cardName)) {
    fail(out);
    return;
  }
  const Card& card = deck.cards.at(cardName);
  std::string upper = card.name;
  for (size_t i = 0; i < upper.size(); ++i) {
    if (upper[i] >= 'a' && upper[i] <= 'z') {
      upper[i] = upper[i] - 'a' + 'A';
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
    const std::string& deckName,
    const std::string& cardName,
    const std::string& type, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  if (!deck.cards.contains(cardName)) {
    fail(out);
    return;
  }
  deck.cards.at(cardName).type = type;
}

void gordejchik::cmdSetDesc(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName,
    const std::string& desc, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck& deck = decks.at(deckName);
  if (!deck.cards.contains(cardName)) {
    fail(out);
    return;
  }
  deck.cards.at(cardName).description = desc;
}

void gordejchik::cmdRange(DeckStore& decks,
    const std::string& deckName,
    const std::string& stat,
    const std::string& minStr,
    const std::string& maxStr, std::ostream& out)
{
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
  const size_t total = deck.cards.size();
  if (total == 0) {
    return;
  }
  std::string* names = new std::string[total];
  try {
    size_t found = 0;
    const bool usePower = (stat == "power");
    using CardCIter = HashTable< std::string, Card >::ConstIterator;
    for (CardCIter it = deck.cards.cbegin(); it != deck.cards.cend(); ++it) {
      const Card& card = it->second;
      const int val = usePower ? card.power : card.cost;
      if (val >= minVal && val <= maxVal) {
        names[found] = it->first;
        ++found;
      }
    }
    std::sort(names, names + found);
    for (size_t i = 0; i < found; ++i) {
      const Card& card = deck.cards.at(names[i]);
      out << names[i] << ": POWER " << card.power
          << ", COST " << card.cost << "\n";
    }
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

void gordejchik::cmdOptimize(DeckStore& decks,
    const std::string& deckName,
    const std::string& budgetStr,
    const std::string& newDeckName, std::ostream& out)
{
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
  const size_t count = deck.cards.size();
  if (count == 0) {
    out << "TOTAL POWER: 0" << "\n";
    return;
  }
  Card** allCards = new Card*[count];
  size_t idx = 0;
  using CardIter = HashTable< std::string, Card >::Iterator;
  for (CardIter it = deck.cards.begin(); it != deck.cards.end(); ++it) {
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
      const Card& card = deck.cards.at(names[i]);
      out << names[i] << ": POWER " << card.power
          << ", COST " << card.cost << "\n";
    }
    out << "TOTAL POWER: " << res.totalPower() << "\n";
    if (!newDeckName.empty()) {
      Deck newDeck{newDeckName, {}};
      for (size_t i = 0; i < res.count(); ++i) {
        newDeck.cards.insert(names[i], deck.cards.at(names[i]));
      }
      decks.insert(newDeckName, newDeck);
    }
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

void gordejchik::cmdBattle(DeckStore& decks,
    const std::string& deck1Name,
    const std::string& deck2Name,
    const std::string& budgetStr, std::ostream& out)
{
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
  const size_t count1 = deck1.cards.size();
  const size_t count2 = deck2.cards.size();
  Card** arr1 = nullptr;
  Card** arr2 = nullptr;
  BackpackResult res1(nullptr, 0, 0);
  BackpackResult res2(nullptr, 0, 0);
  using CardIter = HashTable< std::string, Card >::Iterator;
  try {
    if (count1 > 0) {
      arr1 = new Card*[count1];
      size_t idx = 0;
      for (CardIter it = deck1.cards.begin(); it != deck1.cards.end(); ++it) {
        arr1[idx] = &it->second;
        ++idx;
      }
    }
    if (count2 > 0) {
      arr2 = new Card*[count2];
      size_t idx = 0;
      for (CardIter it = deck2.cards.begin(); it != deck2.cards.end(); ++it) {
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
        const Card& card = deck1.cards.at(names1[i]);
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
        const Card& card = deck2.cards.at(names2[i]);
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
    const std::string& newName,
    const std::string& name1,
    const std::string& name2, std::ostream& out)
{
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
  Deck merged{newName, {}};
  using CardCIter = HashTable< std::string, Card >::ConstIterator;
  for (CardCIter it = d1.cards.cbegin(); it != d1.cards.cend(); ++it) {
    merged.cards.insert(it->first, it->second);
  }
  for (CardCIter it = d2.cards.cbegin(); it != d2.cards.cend(); ++it) {
    if (!merged.cards.contains(it->first)) {
      merged.cards.insert(it->first, it->second);
    }
  }
  decks.insert(newName, merged);
}

void gordejchik::cmdSave(DeckStore& decks,
    const std::string& deckName,
    const std::string& filename, std::ostream& out)
{
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
  const size_t count = deck.cards.size();
  file << count << "\n";
  using CardCIter = HashTable< std::string, Card >::ConstIterator;
  for (CardCIter it = deck.cards.cbegin(); it != deck.cards.cend(); ++it) {
    const Card& card = it->second;
    file << card.name << " "
        << card.power << " "
        << card.cost << "\n";
    file << card.type << "\n";
    file << card.description << "\n";
  }
}

void gordejchik::cmdLoad(DeckStore& decks,
    const std::string& deckName,
    const std::string& filename, std::ostream& out)
{
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
  Deck deck{deckName, {}};
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
    deck.cards.insert(cardName, Card{cardName, power, cost, type, desc});
  }
  decks.insert(deckName, deck);
}

gordejchik::HashTable< std::string, gordejchik::CommandHandler >
gordejchik::makeCommandTable()
{
  HashTable< std::string, CommandHandler > table;
  table.insert("help", cmdHelp);
  table.insert("create", cmdCreate);
  table.insert("delete", cmdDelete);
  table.insert("show", cmdShow);
  return table;
}
