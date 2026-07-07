#ifndef CARD_HPP
#define CARD_HPP

#include <string>

namespace gordejchik {

  struct Card {
    std::string name;
    int power;
    int cost;
    std::string type;
    std::string description;
  };
}
#endif
