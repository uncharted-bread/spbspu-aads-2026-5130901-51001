#ifndef GORDEJCHIK_NAMED_LIST_HPP
#define GORDEJCHIK_NAMED_LIST_HPP

#include <string>
#include "list.hpp"

namespace gordejchik {
  struct NamedList {
    std::string name;
    List< size_t > nums;
  };
}

#endif
