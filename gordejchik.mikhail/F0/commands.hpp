#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <cstddef>
#include <iosfwd>
#include <string>
#include "card.hpp"
#include "gameconfig.hpp"
#include "hashtable.hpp"

namespace gordejchik {

  const size_t MAX_TOKENS = 10;

  struct ParsedCommand {
    std::string tokens[MAX_TOKENS];
    size_t count;
  };

  ParsedCommand parseLine(const std::string& line);

  using Deck = HashTable< std::string, Card >;
  using DeckStore = HashTable< std::string, Deck >;
  using CommandHandler =
      void(*)(DeckStore&, game_config_t&, const ParsedCommand&, std::ostream&);

  HashTable< std::string, CommandHandler > makeCommandTable();

  void cmdCreate(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdDelete(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdHelp(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdShow(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdAdd(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdRemove(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdInfo(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdSetType(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdSetDesc(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdRange(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdOptimize(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdBattle(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdMerge(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdSave(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdLoad(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdTrade(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);
  void cmdConfig(DeckStore& decks, game_config_t& config,
      const ParsedCommand& cmd, std::ostream& out);

}

#endif
