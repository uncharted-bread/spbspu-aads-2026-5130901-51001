#include "dictCommands.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <functional>

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: ./lab filename" << "\n";
    return 1;
  }
  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Не получилось открыть файл: " << argv[1] << "\n";
    return 1;
  }

  gordejchik::DictOfDicts dicts;
  gordejchik::readDicts(dicts, file);
  file.close();

  using CmdFunc = std::function< void(gordejchik::DictOfDicts&,
      std::istream&, std::ostream&) >;
  gordejchik::BSTree< std::string, CmdFunc > commands;

  commands.push("print", [](gordejchik::DictOfDicts& d, std::istream& in, std::ostream& out)
      {
        gordejchik::doPrint(d, in, out);
      });
  commands.push("complement", [](gordejchik::DictOfDicts& d, std::istream& in, std::ostream& out)
      {
        gordejchik::doComplement(d, in, out);
      });
  commands.push("intersect", [](gordejchik::DictOfDicts& d, std::istream& in, std::ostream& out)
      {
        gordejchik::doIntersect(d, in, out);
      });
  commands.push("union", [](gordejchik::DictOfDicts& d, std::istream& in, std::ostream& out)
      {
        gordejchik::doUnion(d, in, out);
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
      std::cout << "<INVALID COMMAND>" << "\n";
      continue;
    }
    commands.get(cmd)(dicts, iss, std::cout);
  }
  return 0;
}
