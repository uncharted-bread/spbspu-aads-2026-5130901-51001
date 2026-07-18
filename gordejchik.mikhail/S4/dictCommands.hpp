#ifndef DICT_COMMANDS_HPP
#define DICT_COMMANDS_HPP

#include <string>
#include <iosfwd>
#include "bsTree.hpp"

namespace gordejchik {
  using Dict = BSTree< int, std::string >;
  using DictOfDicts = BSTree< std::string, Dict >;

  bool readDicts(DictOfDicts& dicts, std::istream& in);
  void doPrint(const DictOfDicts& dicts, std::istream& in, std::ostream& out);
  void doComplement(DictOfDicts& dicts, std::istream& in, std::ostream& out);
  void doIntersect(DictOfDicts& dicts, std::istream& in, std::ostream& out);
  void doUnion(DictOfDicts& dicts, std::istream& in, std::ostream& out);
}

#endif
