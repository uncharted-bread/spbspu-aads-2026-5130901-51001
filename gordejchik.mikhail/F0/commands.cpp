#include "commands.hpp"
#include "backpack.hpp"
#include <algorithm>
#include <fstream>

void gordejchik::cmdCreate(DeckStore& decks,
    const std::string& name, std::ostream& out)
{
  if (decks.contains(name)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  decks.insert(name, new Deck(name));
}

void gordejchik::cmdDelete(DeckStore& decks,
    const std::string& name, std::ostream& out)
{
  if (!decks.contains(name)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Deck* deck = decks.at(name);
  decks.erase(name);
  delete deck;
}

void gordejchik::cmdHelp(std::ostream& out)
{
  out << "create <deck>" << "\n";
  out << "delete <deck>" << "\n";
  out << "add <deck> <card> <power> <cost>" << "\n";
  out << "remove <deck> <card>" << "\n";
  out << "set-type <deck> <card> <type>" << "\n";
  out << "set-desc <deck> <card> <text>" << "\n";
  out << "show <deck>" << "\n";
  out << "info <deck> <card>" << "\n";
  out << "range <deck> <stat> <min> <max>" << "\n";
  out << "merge <new-deck> <deck-1> <deck-2>" << "\n";
  out << "optimize <deck> <budget> [<new-deck>]" << "\n";
  out << "battle <deck-1> <deck-2> <budget>" << "\n";
  out << "save <deck> <filename>" << "\n";
  out << "load <deck> <filename>" << "\n";
  out << "help" << "\n";
}

void gordejchik::cmdAdd(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName,
    const std::string& powerStr,
    const std::string& costStr, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  int power = 0;
  int cost = 0;
  try {
    size_t pos1 = 0;
    power = std::stoi(powerStr, &pos1);
    if (pos1 != powerStr.size()) {
      out << "<INVALID COMMAND>" << "\n";
      return;
    }
    size_t pos2 = 0;
    cost = std::stoi(costStr, &pos2);
    if (pos2 != costStr.size()) {
      out << "<INVALID COMMAND>" << "\n";
      return;
    }
  } catch (...) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Deck* deck = decks.at(deckName);
  if (deck->cards_.contains(cardName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Card* card = new Card{cardName, power, cost, "", ""};
  deck->cards_.insert(cardName, card);
}

void gordejchik::cmdRemove(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Deck* deck = decks.at(deckName);
  if (!deck->cards_.contains(cardName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Card* card = deck->cards_.at(cardName);
  deck->cards_.erase(cardName);
  delete card;
}

void gordejchik::cmdShow(DeckStore& decks,
    const std::string& name, std::ostream& out)
{
  if (!decks.contains(name)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Deck* deck = decks.at(name);
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
}

void gordejchik::cmdInfo(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Deck* deck = decks.at(deckName);
  if (!deck->cards_.contains(cardName)) {
    out << "<INVALID COMMAND>" << "\n";
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
}

void gordejchik::cmdSetType(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName,
    const std::string& type, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Deck* deck = decks.at(deckName);
  if (!deck->cards_.contains(cardName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  deck->cards_.at(cardName)->type_ = type;
}

void gordejchik::cmdSetDesc(DeckStore& decks,
    const std::string& deckName,
    const std::string& cardName,
    const std::string& desc, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Deck* deck = decks.at(deckName);
  if (!deck->cards_.contains(cardName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  deck->cards_.at(cardName)->description_ = desc;
}

void gordejchik::cmdRange(DeckStore& decks,
    const std::string& deckName,
    const std::string& stat,
    const std::string& minStr,
    const std::string& maxStr, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  if (stat != "power" && stat != "cost") {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  int minVal = 0;
  int maxVal = 0;
  try {
    size_t pos1 = 0;
    minVal = std::stoi(minStr, &pos1);
    if (pos1 != minStr.size()) {
      out << "<INVALID COMMAND>" << "\n";
      return;
    }
    size_t pos2 = 0;
    maxVal = std::stoi(maxStr, &pos2);
    if (pos2 != maxStr.size()) {
      out << "<INVALID COMMAND>" << "\n";
      return;
    }
  } catch (...) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  if (minVal > maxVal) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Deck* deck = decks.at(deckName);
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
    return;
  }
  std::sort(names, names + found);
  for (size_t i = 0; i < found; ++i) {
    Card* card = deck->cards_.at(names[i]);
    out << names[i] << ": POWER " << card->power_
        << ", COST " << card->cost_ << "\n";
  }
  delete[] names;
}

void gordejchik::cmdOptimize(DeckStore& decks,
    const std::string& deckName,
    const std::string& budgetStr,
    const std::string& newDeckName, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  if (!newDeckName.empty() && decks.contains(newDeckName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  int budget = 0;
  try {
    size_t pos = 0;
    budget = std::stoi(budgetStr, &pos);
    if (pos != budgetStr.size() || budget < 0) {
      out << "<INVALID COMMAND>" << "\n";
      return;
    }
  } catch (...) {
    out << "<INVALID COMMAND>" << "\n";
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
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  int budget = 0;
  try {
    size_t pos = 0;
    budget = std::stoi(budgetStr, &pos);
    if (pos != budgetStr.size() || budget < 0) {
      out << "<INVALID COMMAND>" << "\n";
      return;
    }
  } catch (...) {
    out << "<INVALID COMMAND>" << "\n";
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
  freeBackpackResult(res1);
  freeBackpackResult(res2);
}

void gordejchik::cmdMerge(DeckStore& decks,
    const std::string& newName,
    const std::string& name1,
    const std::string& name2, std::ostream& out)
{
  if (!decks.contains(name1) || !decks.contains(name2)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  if (decks.contains(newName)) {
    out << "<INVALID COMMAND>" << "\n";
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
}

void gordejchik::cmdSave(DeckStore& decks,
    const std::string& deckName,
    const std::string& filename, std::ostream& out)
{
  if (!decks.contains(deckName)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  Deck* deck = decks.at(deckName);
  std::ofstream file(filename);
  if (!file.is_open()) {
    out << "<INVALID COMMAND>" << "\n";
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
}
