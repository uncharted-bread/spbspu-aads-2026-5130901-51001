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
