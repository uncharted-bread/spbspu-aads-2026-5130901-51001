#include "commands.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>

static const char* TEAL = "\033[38;2;0;128;128m";
static const char* DIM = "\033[2m";
static const char* RESET = "\033[0m";
static const char* BOLD = "\033[1m";

static void printBanner()
{
  std::cerr << TEAL;
  std::cerr << "+----------------------------------------------------------------------+" << "\n";
  std::cerr << "|  " << BOLD << "Добро пожаловать в CustStone - ваша настраиваемая карточная игра"
      << RESET << TEAL << " |" << "\n";
  std::cerr << "|  Введите 'help' для справки                                          |" << "\n";
  std::cerr << "+----------------------------------------------------------------------+" << "\n";
  std::cerr << RESET;
}

static void printPrompt()
{
  std::cerr << TEAL << "> " << RESET;
}

int main()
{
  printBanner();
  gordejchik::DeckStore decks;
  std::string line;
  printPrompt();
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      printPrompt();
      continue;
    }
    gordejchik::ParsedCommand cmd = gordejchik::parseLine(line);
    if (cmd.count_ == 0) {
      printPrompt();
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
      std::cerr << "\033[31m" << ">> Ошибка: неверная команда"
          << "\033[0m" << "\n";
    }
    printPrompt();
  }
  std::cerr << "\n" << DIM << "Выход." << RESET << "\n";
  decks.forEach(
    [](const std::string&, gordejchik::Deck* deck)
    {
      delete deck;
    }
  );
  return 0;
}
