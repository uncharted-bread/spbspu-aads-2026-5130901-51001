#ifndef CARD_HPP
#define CARD_HPP

#include <string>

namespace gordejchik {

  struct Card {
    std::string name_;
    int power_;
    int cost_;
    std::string type_;
    std::string description_;
  };
}
#endif
