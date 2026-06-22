#include "commands.hpp"

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
