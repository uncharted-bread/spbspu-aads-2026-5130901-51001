#include <exception>
#include <iostream>
#include <string>
#include "commands.hpp"

int main()
{
  std::cerr << "CustStone - a card game. "
      << "Type 'help' for the list of commands." << "\n";
  gordejchik::DeckStore decks;
  using CommandTable = gordejchik::HashTable< std::string, gordejchik::CommandHandler >;
  const CommandTable commands = gordejchik::makeCommandTable();
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    gordejchik::ParsedCommand cmd = gordejchik::parseLine(line);
    if (cmd.count == 0) {
      continue;
    }
    CommandTable::ConstIterator it = commands.find(cmd.tokens[0]);
    if (it != commands.cend()) {
      try {
        it->second(decks, cmd, std::cout);
      } catch (const std::exception& e) {
        std::cerr << "Internal error: " << e.what() << "\n";
        return 2;
      }
    } else {
      std::cout << "<INVALID COMMAND>" << "\n";
    }
  }
  return 0;
}
