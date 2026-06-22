#include "commands.hpp"
#include "backpack.hpp"
#include <algorithm>
#include <fstream>
#include <string>

static const char* GREEN = "\033[32m";
static const char* RED = "\033[31m";
static const char* RESET = "\033[0m";

static void ok(const std::string& msg)
{
  std::cerr << GREEN << ">> " << msg << RESET << "\n";
}

static void fail(std::ostream& out)
{
  out << "<INVALID COMMAND>" << "\n";
  std::cerr << RED << ">> Ошибка: неверная команда" << RESET << "\n";
}

static bool parseInt(const std::string& str, int& result)
{
  try {
    size_t pos = 0;
    result = std::stoi(str, &pos);
    if (pos != str.size()) {
      return false;
    }
  } catch (...) {
    return false;
  }
  return true;
}

void gordejchik::cmdCreate(DeckStore& decks,
    const std::string& name, std::ostream& out)
{
  if (decks.contains(name)) {
    fail(out);
    return;
  }
  decks.insert(name, new Deck(name));
  ok("Колода '" + name + "' создана");
}

void gordejchik::cmdDelete(DeckStore& decks,
    const std::string& name, std::ostream& out)
{
  if (!decks.contains(name)) {
    fail(out);
    return;
  }
  Deck* deck = decks.at(name);
  decks.erase(name);
  delete deck;
  ok("Колода '" + name + "' удалена");
}

void gordejchik::cmdHelp(std::ostream& out)
{
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
  ok("Справка выведена");
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
  if (!parseInt(powerStr, power) || !parseInt(costStr, cost)) {
    fail(out);
    return;
  }
  Deck* deck = decks.at(deckName);
  if (deck->cards_.contains(cardName)) {
    fail(out);
    return;
  }
  Card* card = new Card{cardName, power, cost, "", ""};
  deck->cards_.insert(cardName, card);
  ok("Карта '" + cardName + "' добавлена в '" + deckName + "'");
}

void gordejchik::cmdRemove(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck* deck = decks.at(deckName);
  if (!deck->cards_.contains(cardName)) {
    fail(out);
    return;
  }
  Card* card = deck->cards_.at(cardName);
  deck->cards_.erase(cardName);
  delete card;
  ok("Карта '" + cardName + "' удалена из '" + deckName + "'");
}

void gordejchik::cmdShow(DeckStore& decks,
    const std::string& name, std::ostream& out)
{
  if (!decks.contains(name)) {
    fail(out);
    return;
  }
  const Deck* deck = decks.at(name);
  const size_t count = deck->cards_.size();
  if (count == 0) {
    return;
  }
  std::string* names = new std::string[count];
  size_t idx = 0;
  deck->cards_.forEach(
    [&names, &idx](const std::string& key, Card*)
    {
      names[idx] = key;
      ++idx;
    }
  );
  std::sort(names, names + count);
  for (size_t i = 0; i < count; ++i) {
    Card* card = deck->cards_.at(names[i]);
    out << names[i] << ": POWER " << card->power_
        << ", COST " << card->cost_ << "\n";
  }
  delete[] names;
  ok("Колода '" + name + "': " + std::to_string(count) + " карт");
}

void gordejchik::cmdInfo(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck* deck = decks.at(deckName);
  if (!deck->cards_.contains(cardName)) {
    fail(out);
    return;
  }
  const Card* card = deck->cards_.at(cardName);
  std::string upper = card->name_;
  for (size_t i = 0; i < upper.size(); ++i) {
    if (upper[i] >= 'a' && upper[i] <= 'z') {
      upper[i] = upper[i] - 'a' + 'A';
    }
  }
  std::string header = "=== " + upper + " ===";
  out << header << "\n";
  if (card->type_.empty()) {
    out << "TYPE: <none>" << "\n";
  } else {
    out << "TYPE: " << card->type_ << "\n";
  }
  out << "POWER: " << card->power_
      << ", COST: " << card->cost_ << "\n";
  out << "DATA:" << "\n";
  if (card->description_.empty()) {
    out << "<no data>" << "\n";
  } else {
    out << card->description_ << "\n";
  }
  std::string footer(header.size(), '=');
  out << footer << "\n";
  ok("Информация о карте '" + cardName + "'");
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
  Deck* deck = decks.at(deckName);
  if (!deck->cards_.contains(cardName)) {
    fail(out);
    return;
  }
  deck->cards_.at(cardName)->type_ = type;
  ok("Тип карты '" + cardName + "' установлен");
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
  Deck* deck = decks.at(deckName);
  if (!deck->cards_.contains(cardName)) {
    fail(out);
    return;
  }
  deck->cards_.at(cardName)->description_ = desc;
  ok("Описание карты '" + cardName + "' установлено");
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
  if (!parseInt(minStr, minVal) || !parseInt(maxStr, maxVal)) {
    fail(out);
    return;
  }
  if (minVal > maxVal) {
    fail(out);
    return;
  }
  const Deck* deck = decks.at(deckName);
  const size_t total = deck->cards_.size();
  if (total == 0) {
    return;
  }
  std::string* names = new std::string[total];
  size_t found = 0;
  const bool usePower = (stat == "power");
  deck->cards_.forEach(
    [&names, &found, usePower, minVal, maxVal](
        const std::string& key, Card* card)
    {
      const int val = usePower ? card->power_ : card->cost_;
      if (val >= minVal && val <= maxVal) {
        names[found] = key;
        ++found;
      }
    }
  );
  if (found == 0) {
    delete[] names;
    ok("Найдено карт: 0");
    return;
  }
  std::sort(names, names + found);
  for (size_t i = 0; i < found; ++i) {
    Card* card = deck->cards_.at(names[i]);
    out << names[i] << ": POWER " << card->power_
        << ", COST " << card->cost_ << "\n";
  }
  delete[] names;
  ok("Найдено карт: " + std::to_string(found));
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
  if (!parseInt(budgetStr, budget) || budget < 0) {
    fail(out);
    return;
  }
  Deck* deck = decks.at(deckName);
  const size_t count = deck->cards_.size();
  if (count == 0) {
    out << "TOTAL POWER: 0" << "\n";
    return;
  }
  Card** allCards = new Card*[count];
  size_t idx = 0;
  deck->cards_.forEach(
    [&allCards, &idx](const std::string&, Card* card)
    {
      allCards[idx] = card;
      ++idx;
    }
  );
  BackpackResult res = solveBackpack(allCards, count, budget);
  delete[] allCards;
  std::string* names = new std::string[res.count_];
  for (size_t i = 0; i < res.count_; ++i) {
    names[i] = res.cards_[i]->name_;
  }
  std::sort(names, names + res.count_);
  for (size_t i = 0; i < res.count_; ++i) {
    Card* card = deck->cards_.at(names[i]);
    out << names[i] << ": POWER " << card->power_
        << ", COST " << card->cost_ << "\n";
  }
  out << "TOTAL POWER: " << res.totalPower_ << "\n";
  ok("Оптимизация завершена, бюджет: " + budgetStr);
  if (!newDeckName.empty()) {
    Deck* newDeck = new Deck(newDeckName);
    for (size_t i = 0; i < res.count_; ++i) {
      Card* src = deck->cards_.at(names[i]);
      Card* copy = new Card{src->name_, src->power_,
          src->cost_, src->type_, src->description_};
      newDeck->cards_.insert(copy->name_, copy);
    }
    decks.insert(newDeckName, newDeck);
  }
  delete[] names;
  freeBackpackResult(res);
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
  if (!parseInt(budgetStr, budget) || budget < 0) {
    fail(out);
    return;
  }
  Deck* deck1 = decks.at(deck1Name);
  Deck* deck2 = decks.at(deck2Name);
  const size_t count1 = deck1->cards_.size();
  const size_t count2 = deck2->cards_.size();
  Card** arr1 = nullptr;
  Card** arr2 = nullptr;
  if (count1 > 0) {
    arr1 = new Card*[count1];
    size_t idx = 0;
    deck1->cards_.forEach(
      [&arr1, &idx](const std::string&, Card* card)
      {
        arr1[idx] = card;
        ++idx;
      }
    );
  }
  if (count2 > 0) {
    arr2 = new Card*[count2];
    size_t idx = 0;
    deck2->cards_.forEach(
      [&arr2, &idx](const std::string&, Card* card)
      {
        arr2[idx] = card;
        ++idx;
      }
    );
  }
  BackpackResult res1 = solveBackpack(arr1, count1, budget);
  BackpackResult res2 = solveBackpack(arr2, count2, budget);
  delete[] arr1;
  delete[] arr2;
  out << "=== " << deck1Name << " ===" << "\n";
  if (res1.count_ > 0) {
    std::string* names1 = new std::string[res1.count_];
    for (size_t i = 0; i < res1.count_; ++i) {
      names1[i] = res1.cards_[i]->name_;
    }
    std::sort(names1, names1 + res1.count_);
    for (size_t i = 0; i < res1.count_; ++i) {
      Card* card = deck1->cards_.at(names1[i]);
      out << names1[i] << ": POWER " << card->power_
          << ", COST " << card->cost_ << "\n";
    }
    delete[] names1;
  }
  out << "TOTAL POWER: " << res1.totalPower_ << "\n";
  out << "=== " << deck2Name << " ===" << "\n";
  if (res2.count_ > 0) {
    std::string* names2 = new std::string[res2.count_];
    for (size_t i = 0; i < res2.count_; ++i) {
      names2[i] = res2.cards_[i]->name_;
    }
    std::sort(names2, names2 + res2.count_);
    for (size_t i = 0; i < res2.count_; ++i) {
      Card* card = deck2->cards_.at(names2[i]);
      out << names2[i] << ": POWER " << card->power_
          << ", COST " << card->cost_ << "\n";
    }
    delete[] names2;
  }
  out << "TOTAL POWER: " << res2.totalPower_ << "\n";
  if (res1.totalPower_ > res2.totalPower_) {
    out << "WINNER: " << deck1Name << "\n";
  } else if (res2.totalPower_ > res1.totalPower_) {
    out << "WINNER: " << deck2Name << "\n";
  } else {
    out << "DRAW" << "\n";
  }
  ok("Битва окончена");
  freeBackpackResult(res1);
  freeBackpackResult(res2);
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
  Deck* d1 = decks.at(name1);
  Deck* d2 = decks.at(name2);
  Deck* merged = new Deck(newName);
  d1->cards_.forEach(
    [&merged](const std::string&, Card* card)
    {
      Card* copy = new Card{card->name_, card->power_,
          card->cost_, card->type_, card->description_};
      merged->cards_.insert(copy->name_, copy);
    }
  );
  d2->cards_.forEach(
    [&merged](const std::string&, Card* card)
    {
      if (!merged->cards_.contains(card->name_)) {
        Card* copy = new Card{card->name_, card->power_,
            card->cost_, card->type_, card->description_};
        merged->cards_.insert(copy->name_, copy);
      }
    }
  );
  decks.insert(newName, merged);
  ok("Колоды объединены в '" + newName + "'");
}

void gordejchik::cmdSave(DeckStore& decks,
    const std::string& deckName,
    const std::string& filename, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    fail(out);
    return;
  }
  Deck* deck = decks.at(deckName);
  std::ofstream file(filename);
  if (!file.is_open()) {
    fail(out);
    return;
  }
  const size_t count = deck->cards_.size();
  file << count << "\n";
  deck->cards_.forEach(
    [&file](const std::string&, Card* card)
    {
      file << card->name_ << " "
          << card->power_ << " "
          << card->cost_ << "\n";
      file << card->type_ << "\n";
      file << card->description_ << "\n";
    }
  );
  ok("Колода сохранена в '" + filename + "'");
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
  Deck* deck = new Deck(deckName);
  for (size_t i = 0; i < count; ++i) {
    std::string header;
    if (!std::getline(file, header)) {
      delete deck;
      fail(out);
      return;
    }
    size_t space1 = header.find(' ');
    if (space1 == std::string::npos) {
      delete deck;
      fail(out);
      return;
    }
    size_t space2 = header.find(' ', space1 + 1);
    if (space2 == std::string::npos) {
      delete deck;
      fail(out);
      return;
    }
    std::string cardName = header.substr(0, space1);
    std::string powerStr = header.substr(space1 + 1,
        space2 - space1 - 1);
    std::string costStr = header.substr(space2 + 1);
    int power = 0;
    int cost = 0;
    if (!parseInt(powerStr, power)
        || !parseInt(costStr, cost)) {
      delete deck;
      fail(out);
      return;
    }
    std::string type;
    std::string desc;
    if (!std::getline(file, type)
        || !std::getline(file, desc)) {
      delete deck;
      fail(out);
      return;
    }
    Card* card = new Card{cardName, power, cost, type, desc};
    deck->cards_.insert(cardName, card);
  }
  decks.insert(deckName, deck);
  ok("Колода загружена из '" + filename + "'");
}
