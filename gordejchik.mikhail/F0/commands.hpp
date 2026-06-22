#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "hashtable.hpp"
#include "deck.hpp"
#include <string>
#include <iostream>

namespace gordejchik {

  using DeckStore = HashTable< std::string, Deck* >;

  void cmdCreate(DeckStore& decks, const std::string& name,
      std::ostream& out);
  void cmdDelete(DeckStore& decks, const std::string& name,
      std::ostream& out);
  void cmdHelp(std::ostream& out);
  void cmdAdd(DeckStore& decks, const std::string& deckName,
    const std::string& cardName, const std::string& powerStr,
    const std::string& costStr, std::ostream& out);
  void cmdRemove(DeckStore& decks, const std::string& deckName,
    const std::string& cardName, std::ostream& out);
}

#endif
