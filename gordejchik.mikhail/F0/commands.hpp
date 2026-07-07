#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iosfwd>
#include <string>
#include "deck.hpp"
#include "hashtable.hpp"
#include "parser.hpp"

namespace gordejchik {

  using DeckStore = HashTable< std::string, Deck >;
  using CommandHandler = void(*)(DeckStore&, const ParsedCommand&, std::ostream&);

  HashTable< std::string, CommandHandler > makeCommandTable();

  void cmdCreate(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdDelete(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdHelp(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdShow(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdAdd(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdRemove(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdInfo(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdSetType(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdSetDesc(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdRange(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdOptimize(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdBattle(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdMerge(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdSave(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);
  void cmdLoad(DeckStore& decks, const ParsedCommand& cmd, std::ostream& out);

}

#endif
