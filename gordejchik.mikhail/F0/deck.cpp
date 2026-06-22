#include "deck.hpp"

gordejchik::Deck::Deck(const std::string& name):
  name_(name),
  cards_()
{}

gordejchik::Deck::~Deck()
{
  cards_.forEach(
    [](const std::string&, Card* card)
    {
      delete card;
    }
  );
}
