#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "dictCommands.hpp"

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: ./lab filename\n";
    return 1;
  }
  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Cannot open file: " << argv[1] << "\n";
    return 1;
  }

  try {
    gordejchik::DictOfDicts dicts;
    if (!gordejchik::readDicts(dicts, file)) {
      std::cerr << "Invalid dictionary data in file: " << argv[1] << "\n";
      return 1;
    }
    file.close();

    using CommandFunc = void (*)(gordejchik::DictOfDicts&, std::istream&, std::ostream&);
    gordejchik::BSTree< std::string, CommandFunc > commands;
    commands.push("print",
        [](gordejchik::DictOfDicts& dicts, std::istream& in, std::ostream& out)
        {
          gordejchik::doPrint(dicts, in, out);
        });
    commands.push("complement",
        [](gordejchik::DictOfDicts& dicts, std::istream& in, std::ostream& out)
        {
          gordejchik::doComplement(dicts, in, out);
        });
    commands.push("intersect",
        [](gordejchik::DictOfDicts& dicts, std::istream& in, std::ostream& out)
        {
          gordejchik::doIntersect(dicts, in, out);
        });
    commands.push("union",
        [](gordejchik::DictOfDicts& dicts, std::istream& in, std::ostream& out)
        {
          gordejchik::doUnion(dicts, in, out);
        });

    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty()) {
        continue;
      }
      std::istringstream iss(line);
      std::string cmd;
      iss >> cmd;
      if (!commands.contains(cmd)) {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }
      commands.get(cmd)(dicts, iss, std::cout);
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return 2;
  }
  return 0;
}
