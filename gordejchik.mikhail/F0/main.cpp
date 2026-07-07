#include <iostream>
#include <string>
#include "commands.hpp"
#include "parser.hpp"

int main()
{
  gordejchik::DeckStore decks;
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    gordejchik::ParsedCommand cmd = gordejchik::parseLine(line);
    if (cmd.count_ == 0) {
      continue;
    }
    const std::string& name = cmd.tokens_[0];
    if (name == "help" && cmd.count_ == 1) {
      gordejchik::cmdHelp(std::cout);
    } else if (name == "create" && cmd.count_ == 2) {
      gordejchik::cmdCreate(decks, cmd.tokens_[1], std::cout);
    } else if (name == "delete" && cmd.count_ == 2) {
      gordejchik::cmdDelete(decks, cmd.tokens_[1], std::cout);
    } else if (name == "add" && cmd.count_ == 5) {
      gordejchik::cmdAdd(decks, cmd.tokens_[1], cmd.tokens_[2],
          cmd.tokens_[3], cmd.tokens_[4], std::cout);
    } else if (name == "remove" && cmd.count_ == 3) {
      gordejchik::cmdRemove(decks, cmd.tokens_[1],
          cmd.tokens_[2], std::cout);
    } else if (name == "show" && cmd.count_ == 2) {
      gordejchik::cmdShow(decks, cmd.tokens_[1], std::cout);
    } else if (name == "info" && cmd.count_ == 3) {
      gordejchik::cmdInfo(decks, cmd.tokens_[1],
          cmd.tokens_[2], std::cout);
    } else if (name == "set-type" && cmd.count_ == 4) {
      gordejchik::cmdSetType(decks, cmd.tokens_[1],
          cmd.tokens_[2], cmd.tokens_[3], std::cout);
    } else if (name == "set-desc" && cmd.count_ == 4) {
      gordejchik::cmdSetDesc(decks, cmd.tokens_[1],
          cmd.tokens_[2], cmd.tokens_[3], std::cout);
    } else if (name == "range" && cmd.count_ == 5) {
      gordejchik::cmdRange(decks, cmd.tokens_[1],
          cmd.tokens_[2], cmd.tokens_[3], cmd.tokens_[4],
          std::cout);
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
  decks.forEach(
    [](const std::string&, gordejchik::Deck* deck)
    {
      delete deck;
    }
  );
  return 0;
}
