#include "deck.hpp"

gordejchik::Deck::Deck(const std::string& name):
  name_(name),
  cards_()
{}

gordejchik::Deck::~Deck()
{
  using CardIter = HashTable< std::string, Card* >::Iterator;
  for (CardIter it = cards_.begin(); it != cards_.end(); ++it) {
    delete it->second;
  }
}
