#ifndef BACKPACK_HPP
#define BACKPACK_HPP

#include "card.hpp"
#include <cstddef>

namespace gordejchik {

  struct BackpackResult {
    Card** cards_;
    size_t count_;
    int totalPower_;
  };

  BackpackResult solveBackpack(Card** allCards, size_t cardCount, int budget);
  void freeBackpackResult(BackpackResult& result);
}

#endif
