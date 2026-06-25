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

static bool readThreeNames(std::istream& in, std::string& newName,
    std::string& name1, std::string& name2)
{
  return static_cast< bool >(in >> newName >> name1 >> name2);
}

static bool validateSetOp(const gordejchik::DictOfDicts& dicts,
    const std::string& newName, const std::string& name1,
    const std::string& name2, std::ostream& out)
{
  if (dicts.contains(newName) || !dicts.contains(name1)
      || !dicts.contains(name2)) {
    out << "<INVALID COMMAND>" << "\n";
    return false;
  }
  return true;
}

void gordejchik::doComplement(DictOfDicts& dicts, std::istream& in,
    std::ostream& out)
{
  std::string newName, name1, name2;
  if (!readThreeNames(in, newName, name1, name2)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  if (!validateSetOp(dicts, newName, name1, name2, out)) {
    return;
  }
  const Dict& d1 = dicts.get(name1);
  const Dict& d2 = dicts.get(name2);
  Dict result;
  for (Dict::const_iterator it = d1.cbegin(); it != d1.cend(); ++it) {
    if (!d2.contains(it->first)) {
      result.push(it->first, it->second);
    }
  }
  dicts.push(newName, result);
}

void gordejchik::doIntersect(DictOfDicts& dicts, std::istream& in,
    std::ostream& out)
{
  std::string newName, name1, name2;
  if (!readThreeNames(in, newName, name1, name2)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  if (!validateSetOp(dicts, newName, name1, name2, out)) {
    return;
  }
  const Dict& d1 = dicts.get(name1);
  const Dict& d2 = dicts.get(name2);
  Dict result;
  for (Dict::const_iterator it = d1.cbegin(); it != d1.cend(); ++it) {
    if (d2.contains(it->first)) {
      result.push(it->first, it->second);
    }
  }
  dicts.push(newName, result);
}

void gordejchik::doUnion(DictOfDicts& dicts, std::istream& in,
    std::ostream& out)
{
  std::string newName, name1, name2;
  if (!readThreeNames(in, newName, name1, name2)) {
    out << "<INVALID COMMAND>" << "\n";
    return;
  }
  if (!validateSetOp(dicts, newName, name1, name2, out)) {
    return;
  }
  const Dict& d1 = dicts.get(name1);
  const Dict& d2 = dicts.get(name2);
  Dict result;
  for (Dict::const_iterator it = d2.cbegin(); it != d2.cend(); ++it) {
    result.push(it->first, it->second);
  }
  for (Dict::const_iterator it = d1.cbegin(); it != d1.cend(); ++it) {
    result.push(it->first, it->second);
  }
  dicts.push(newName, result);
}
