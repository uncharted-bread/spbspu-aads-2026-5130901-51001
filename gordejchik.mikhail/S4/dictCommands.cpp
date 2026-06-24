#include "dictCommands.hpp"
#include <iostream>
#include <sstream>
#include <string>

void gordejchik::readDicts(DictOfDicts& dicts, std::istream& in)
{
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) {
      continue;
    }
    std::istringstream iss(line);
    std::string name;
    iss >> name;
    Dict dict;
    int key = 0;
    std::string value;
    while (iss >> key >> value) {
      dict.push(key, value);
    }
    dicts.push(name, dict);
  }
}

void gordejchik::doPrint(const DictOfDicts& dicts, std::istream& in,
    std::ostream& out)
{
  std::string name;
  if (!(in >> name)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  if (!dicts.contains(name)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  const Dict& dict = dicts.get(name);
  if (dict.empty()) {
    out << "<EMPTY>" << "\n";
    return;
  }
  out << name;
  for (Dict::const_iterator it = dict.cbegin(); it != dict.cend(); ++it) {
    out << " " << it->first << " " << it->second;
  }
  out << "\n";
}
