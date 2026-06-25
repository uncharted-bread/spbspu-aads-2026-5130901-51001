#ifndef DICT_COMMANDS_HPP
#define DICT_COMMANDS_HPP

#include "bsTree.hpp"
#include <string>
#include <iosfwd>
#include <functional>

namespace gordejchik {
  using Dict = BSTree< int, std::string >;
  using DictOfDicts = BSTree< std::string, Dict >;

  void readDicts(DictOfDicts& dicts, std::istream& in);
  void doPrint(const DictOfDicts& dicts, std::istream& in, std::ostream& out);
  void doComplement(DictOfDicts& dicts, std::istream& in, std::ostream& out);
  void doIntersect(DictOfDicts& dicts, std::istream& in, std::ostream& out);
  void doUnion(DictOfDicts& dicts, std::istream& in, std::ostream& out);
}

#endif
