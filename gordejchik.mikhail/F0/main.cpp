#include "commands.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>

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
    if (name == "create" && cmd.count_ == 2) {
      gordejchik::cmdCreate(decks, cmd.tokens_[1], std::cout);
    } else if (name == "delete" && cmd.count_ == 2) {
      gordejchik::cmdDelete(decks, cmd.tokens_[1], std::cout);
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
