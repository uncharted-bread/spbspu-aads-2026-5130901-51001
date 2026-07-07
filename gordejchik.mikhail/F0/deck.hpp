#ifndef DECK_HPP
#define DECK_HPP

#include <string>
#include "card.hpp"
#include "hashtable.hpp"

namespace gordejchik {

  class Deck {
  public:
    std::string name_;
    HashTable< std::string, Card* > cards_;

    explicit Deck(const std::string& name);
    ~Deck();

    Deck(const Deck&) = delete;
    Deck& operator=(const Deck&) = delete;
  };

}

#endif
