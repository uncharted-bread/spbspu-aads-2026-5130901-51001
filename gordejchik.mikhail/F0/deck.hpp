#ifndef DECK_HPP
#define DECK_HPP

#include <string>
#include "card.hpp"
#include "hashtable.hpp"

namespace gordejchik {

  struct Deck {
    std::string name;
    HashTable< std::string, Card > cards;
  };

}

#endif
