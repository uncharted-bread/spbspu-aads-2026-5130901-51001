#ifndef GAMECONFIG_HPP
#define GAMECONFIG_HPP

#include <string>
#include "hashtable.hpp"

namespace gordejchik {

  // Dominance rules, keyed "<type-a>><type-b>"; the bool value is unused.
  using GameRules = HashTable< std::string, bool >;

  const int DEFAULT_BONUS_VALUE = 2;

  struct game_config_t {
    bool bonusEnabled;
    int bonusValue;
    GameRules rules;
  };
}

#endif
