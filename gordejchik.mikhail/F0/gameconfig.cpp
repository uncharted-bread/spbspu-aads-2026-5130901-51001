#include "gameconfig.hpp"
#include <string>
#include <utility>

std::pair< int, int > gordejchik::computeTypeBonuses(
    const BackpackResult& side1, const BackpackResult& side2,
    const game_config_t& config)
{
  int bonus1 = 0;
  int bonus2 = 0;
  if (!config.bonusEnabled || config.rules.empty()) {
    return std::make_pair(bonus1, bonus2);
  }
  for (size_t i = 0; i < side1.count(); ++i) {
    const std::string& typeA = side1.cards()[i]->type;
    if (typeA.empty()) {
      continue;
    }
    for (size_t j = 0; j < side2.count(); ++j) {
      const std::string& typeB = side2.cards()[j]->type;
      if (typeB.empty()) {
        continue;
      }
      if (config.rules.contains(typeA + ">" + typeB)) {
        bonus1 += config.bonusValue;
      }
      if (config.rules.contains(typeB + ">" + typeA)) {
        bonus2 += config.bonusValue;
      }
    }
  }
  return std::make_pair(bonus1, bonus2);
}
