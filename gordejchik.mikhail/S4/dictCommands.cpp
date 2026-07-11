#include "dictCommands.hpp"
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

static void printInvalid(std::ostream& out)
{
  out << "<INVALID COMMAND>\n";
}

static bool hasNoExtraArgs(std::istream& in)
{
  std::string extra;
  return !(in >> extra);
}

static bool readSetOpNames(std::istream& in, std::string& newName,
    std::string& name1, std::string& name2)
{
  return static_cast< bool >(in >> newName >> name1 >> name2) && hasNoExtraArgs(in);
}

static bool isSetOpValid(const gordejchik::DictOfDicts& dicts,
    const std::string& newName, const std::string& name1, const std::string& name2)
{
  return !dicts.contains(newName) && dicts.contains(name1) && dicts.contains(name2);
}

bool gordejchik::readDicts(DictOfDicts& dicts, std::istream& in)
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
    while (iss >> key) {
      std::string value;
      if (!(iss >> value)) {
        return false;
      }
      dict.push(key, value);
    }
    if (!iss.eof()) {
      return false;
    }
    dicts[name].swap(dict);
  }
  return true;
}

void gordejchik::doPrint(const DictOfDicts& dicts, std::istream& in,
    std::ostream& out)
{
  std::string name;
  if (!(in >> name) || !hasNoExtraArgs(in) || !dicts.contains(name)) {
    printInvalid(out);
    return;
  }
  const Dict& dict = dicts.get(name);
  if (dict.empty()) {
    out << "<EMPTY>\n";
    return;
  }
  out << name;
  for (Dict::const_iterator it = dict.cbegin(); it != dict.cend(); ++it) {
    out << " " << it->first << " " << it->second;
  }
  out << "\n";
}

void gordejchik::doComplement(DictOfDicts& dicts, std::istream& in,
    std::ostream& out)
{
  std::string newName;
  std::string name1;
  std::string name2;
  const bool argsOk = readSetOpNames(in, newName, name1, name2);
  if (!argsOk || !isSetOpValid(dicts, newName, name1, name2)) {
    printInvalid(out);
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
  dicts[newName].swap(result);
}

void gordejchik::doIntersect(DictOfDicts& dicts, std::istream& in,
    std::ostream& out)
{
  std::string newName;
  std::string name1;
  std::string name2;
  const bool argsOk = readSetOpNames(in, newName, name1, name2);
  if (!argsOk || !isSetOpValid(dicts, newName, name1, name2)) {
    printInvalid(out);
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
  dicts[newName].swap(result);
}

void gordejchik::doUnion(DictOfDicts& dicts, std::istream& in,
    std::ostream& out)
{
  std::string newName;
  std::string name1;
  std::string name2;
  const bool argsOk = readSetOpNames(in, newName, name1, name2);
  if (!argsOk || !isSetOpValid(dicts, newName, name1, name2)) {
    printInvalid(out);
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
  dicts[newName].swap(result);
}
