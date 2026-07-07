#include <exception>
#include <iostream>
#include <string>
#include "commands.hpp"
#include "parser.hpp"

int main()
{
  gordejchik::DeckStore decks;
  using CommandTable = gordejchik::HashTable< std::string, gordejchik::CommandHandler >;
  const CommandTable commands = gordejchik::makeCommandTable();
  std::string line;
  try {
    while (std::getline(std::cin, line)) {
      if (line.empty()) {
        continue;
      }
      gordejchik::ParsedCommand cmd = gordejchik::parseLine(line);
      if (cmd.count_ == 0) {
        continue;
      }
      const std::string& name = cmd.tokens_[0];
      if (commands.contains(name)) {
        commands.at(name)(decks, cmd, std::cout);
      } else if (name == "optimize" && cmd.count_ == 3) {
        gordejchik::cmdOptimize(decks, cmd.tokens_[1],
            cmd.tokens_[2], "", std::cout);
      } else if (name == "optimize" && cmd.count_ == 4) {
        gordejchik::cmdOptimize(decks, cmd.tokens_[1],
            cmd.tokens_[2], cmd.tokens_[3], std::cout);
      } else if (name == "battle" && cmd.count_ == 4) {
        gordejchik::cmdBattle(decks, cmd.tokens_[1],
            cmd.tokens_[2], cmd.tokens_[3], std::cout);
      } else if (name == "merge" && cmd.count_ == 4) {
        gordejchik::cmdMerge(decks, cmd.tokens_[1],
            cmd.tokens_[2], cmd.tokens_[3], std::cout);
      } else if (name == "save" && cmd.count_ == 3) {
        gordejchik::cmdSave(decks, cmd.tokens_[1],
            cmd.tokens_[2], std::cout);
      } else if (name == "load" && cmd.count_ == 3) {
        gordejchik::cmdLoad(decks, cmd.tokens_[1],
            cmd.tokens_[2], std::cout);
      } else {
        std::cout << "<INVALID COMMAND>" << "\n";
      }
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return 2;
  }
  return 0;
}
