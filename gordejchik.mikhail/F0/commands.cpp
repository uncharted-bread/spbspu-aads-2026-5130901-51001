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
