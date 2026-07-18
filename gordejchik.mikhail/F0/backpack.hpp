#ifndef BACKPACK_HPP
#define BACKPACK_HPP

#include <cstddef>
#include "card.hpp"

namespace gordejchik {

  class BackpackResult {
  public:
    BackpackResult(const BackpackResult&) = delete;
    BackpackResult(BackpackResult&& other) noexcept;
    BackpackResult(Card** cards, size_t count, int totalPower);
    ~BackpackResult();

    BackpackResult& operator=(const BackpackResult&) = delete;
    BackpackResult& operator=(BackpackResult&& other) noexcept;

    Card* const* cards() const;
    size_t count() const;
    int totalPower() const;

  private:
    Card** cards_;
    size_t count_;
    int totalPower_;
  };

  BackpackResult solveBackpack(Card** allCards, size_t cardCount, int budget);
}

#endif
