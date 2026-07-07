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
