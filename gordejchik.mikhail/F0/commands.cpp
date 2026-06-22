#include "commands.hpp"
#include <algorithm>

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
