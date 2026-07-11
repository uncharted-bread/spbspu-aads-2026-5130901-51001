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
  try {
    while (std::getline(std::cin, line)) {
      if (line.empty()) {
        continue;
      }
      gordejchik::ParsedCommand cmd = gordejchik::parseLine(line);
      if (cmd.count == 0) {
        continue;
      }
      const std::string& name = cmd.tokens[0];
      if (commands.contains(name)) {
        commands.at(name)(decks, cmd, std::cout);
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
