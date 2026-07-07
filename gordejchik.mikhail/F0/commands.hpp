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
  void cmdAdd(DeckStore& decks, const std::string& deckName,
      const std::string& cardName, const std::string& powerStr,
      const std::string& costStr, std::ostream& out);
  void cmdRemove(DeckStore& decks, const std::string& deckName,
      const std::string& cardName, std::ostream& out);
  void cmdInfo(DeckStore& decks, const std::string& deckName,
      const std::string& cardName, std::ostream& out);
  void cmdSetType(DeckStore& decks, const std::string& deckName,
      const std::string& cardName, const std::string& type,
      std::ostream& out);
  void cmdSetDesc(DeckStore& decks, const std::string& deckName,
      const std::string& cardName, const std::string& desc,
      std::ostream& out);
  void cmdRange(DeckStore& decks, const std::string& deckName,
      const std::string& stat, const std::string& minStr,
      const std::string& maxStr, std::ostream& out);
  void cmdOptimize(DeckStore& decks,
      const std::string& deckName,
      const std::string& budgetStr,
      const std::string& newDeckName, std::ostream& out);
  void cmdBattle(DeckStore& decks,
      const std::string& deck1Name,
      const std::string& deck2Name,
      const std::string& budgetStr, std::ostream& out);
  void cmdMerge(DeckStore& decks,
      const std::string& newName,
      const std::string& name1,
      const std::string& name2, std::ostream& out);
  void cmdSave(DeckStore& decks, const std::string& deckName,
      const std::string& filename, std::ostream& out);
  void cmdLoad(DeckStore& decks, const std::string& deckName,
      const std::string& filename, std::ostream& out);

}

#endif
