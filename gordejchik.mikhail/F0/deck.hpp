#ifndef DECK_HPP
#define DECK_HPP

#include "hashtable.hpp"
#include "card.hpp"
#include <string>

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
