#include "backpack.hpp"

gordejchik::BackpackResult gordejchik::solveBackpack(
    Card** allCards, size_t cardCount, int budget)
{
  BackpackResult result{nullptr, 0, 0};
  if (cardCount == 0 || budget <= 0) {
    return result;
  }
  const size_t n = cardCount;
  const size_t w = static_cast< size_t >(budget);
  const size_t cols = w + 1;
  int** dp = new int*[n + 1];
  for (size_t i = 0; i <= n; ++i) {
    dp[i] = new int[cols]();
  }
  for (size_t i = 1; i <= n; ++i) {
    const int cost = allCards[i - 1]->cost_;
    const int power = allCards[i - 1]->power_;
    for (size_t j = 0; j <= w; ++j) {
      dp[i][j] = dp[i - 1][j];
      if (cost >= 0
          && static_cast< size_t >(cost) <= j
          && dp[i - 1][j - cost] + power > dp[i][j]) {
        dp[i][j] = dp[i - 1][j - cost] + power;
      }
    }
  }
  result.totalPower_ = dp[n][w];
  bool* taken = new bool[n]();
  size_t j = w;
  for (size_t i = n; i >= 1; --i) {
    if (dp[i][j] != dp[i - 1][j]) {
      taken[i - 1] = true;
      const int cost = allCards[i - 1]->cost_;
      j -= static_cast< size_t >(cost);
      ++result.count_;
    }
  }
  result.cards_ = new Card*[result.count_];
  size_t idx = 0;
  for (size_t i = 0; i < n; ++i) {
    if (taken[i]) {
      result.cards_[idx] = allCards[i];
      ++idx;
    }
  }
  delete[] taken;
  for (size_t i = 0; i <= n; ++i) {
    delete[] dp[i];
  }
  delete[] dp;
  return result;
}

void gordejchik::freeBackpackResult(BackpackResult& result)
{
  delete[] result.cards_;
  result.cards_ = nullptr;
  result.count_ = 0;
  result.totalPower_ = 0;
}
