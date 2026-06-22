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
}

#endif
