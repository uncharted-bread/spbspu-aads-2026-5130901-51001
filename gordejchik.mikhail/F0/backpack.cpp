#include "backpack.hpp"

gordejchik::BackpackResult::BackpackResult(BackpackResult&& other) noexcept:
  cards_(other.cards_),
  count_(other.count_),
  totalPower_(other.totalPower_)
{
  other.cards_ = nullptr;
  other.count_ = 0;
  other.totalPower_ = 0;
}

gordejchik::BackpackResult::BackpackResult(Card** cards, size_t count, int totalPower):
  cards_(cards),
  count_(count),
  totalPower_(totalPower)
{}

gordejchik::BackpackResult::~BackpackResult()
{
  delete[] cards_;
}

gordejchik::BackpackResult& gordejchik::BackpackResult::operator=(BackpackResult&& other) noexcept
{
  if (this != &other) {
    delete[] cards_;
    cards_ = other.cards_;
    count_ = other.count_;
    totalPower_ = other.totalPower_;
    other.cards_ = nullptr;
    other.count_ = 0;
    other.totalPower_ = 0;
  }
  return *this;
}

gordejchik::Card* const* gordejchik::BackpackResult::cards() const
{
  return cards_;
}

size_t gordejchik::BackpackResult::count() const
{
  return count_;
}

int gordejchik::BackpackResult::totalPower() const
{
  return totalPower_;
}

gordejchik::BackpackResult gordejchik::solveBackpack(
    Card** allCards, size_t cardCount, int budget)
{
  if (cardCount == 0 || budget <= 0) {
    return BackpackResult(nullptr, 0, 0);
  }
  const size_t n = cardCount;
  const size_t w = static_cast< size_t >(budget);
  const size_t cols = w + 1;
  int* dp = new int[(n + 1) * cols]();
  for (size_t i = 1; i <= n; ++i) {
    const int cost = allCards[i - 1]->cost;
    const int power = allCards[i - 1]->power;
    for (size_t j = 0; j <= w; ++j) {
      dp[i * cols + j] = dp[(i - 1) * cols + j];
      if (cost >= 0
          && static_cast< size_t >(cost) <= j
          && dp[(i - 1) * cols + j - cost] + power > dp[i * cols + j]) {
        dp[i * cols + j] = dp[(i - 1) * cols + j - cost] + power;
      }
    }
  }
  const int totalPower = dp[n * cols + w];
  bool* taken = nullptr;
  Card** chosen = nullptr;
  size_t takenCount = 0;
  try {
    taken = new bool[n]();
    size_t j = w;
    for (size_t i = n; i >= 1; --i) {
      if (dp[i * cols + j] != dp[(i - 1) * cols + j]) {
        taken[i - 1] = true;
        const int cost = allCards[i - 1]->cost;
        j -= static_cast< size_t >(cost);
        ++takenCount;
      }
    }
    chosen = new Card*[takenCount];
  } catch (...) {
    delete[] taken;
    delete[] dp;
    throw;
  }
  size_t idx = 0;
  for (size_t i = 0; i < n; ++i) {
    if (taken[i]) {
      chosen[idx] = allCards[i];
      ++idx;
    }
  }
  delete[] taken;
  delete[] dp;
  return BackpackResult(chosen, takenCount, totalPower);
}
