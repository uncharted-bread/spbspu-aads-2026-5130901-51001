#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include "list.hpp"
#include "named-list.hpp"

namespace gordejchik {
  static void readInput(List< NamedList >& namedLists)
  {
    std::string name;
    while (std::cin >> name) {
      NamedList namedList{name, {}};
      if (std::cin.eof()) {
        namedLists.pushBack(std::move(namedList));
        break;
      }
      size_t num = 0;
      while (std::cin >> num) {
        namedList.nums.pushBack(num);
      }
      namedLists.pushBack(std::move(namedList));
      std::cin.clear();
    }
  }

  static void printNames(const List< NamedList >& namedLists)
  {
    using NamedCIter = List< NamedList >::const_iterator;
    bool first = true;
    for (NamedCIter it = namedLists.cbegin(); it != namedLists.cend(); ++it) {
      if (!first) {
        std::cout << ' ';
      }
      std::cout << it->name;
      first = false;
    }
    std::cout << '\n';
  }

  static size_t checkedSum(size_t a, size_t b)
  {
    const size_t maxVal = std::numeric_limits< size_t >::max();
    if (maxVal - a < b) {
      throw std::overflow_error("overflow when calculating row sums");
    }
    return a + b;
  }

  static void processZip(List< NamedList >& namedLists, List< size_t >& rowSums)
  {
    using NumIter = List< size_t >::iterator;
    using NumCIter = List< size_t >::const_iterator;
    using NamedIter = List< NamedList >::iterator;
    List< NumIter > iters;
    for (NamedIter it = namedLists.begin(); it != namedLists.end(); ++it) {
      iters.pushBack(it->nums.begin());
    }
    while (true) {
      bool rowHasData = false;
      List< size_t > rowVals;
      List< NumIter >::iterator iterIt = iters.begin();
      NamedIter namedListIt = namedLists.begin();
      while (namedListIt != namedLists.end()) {
        if (*iterIt != namedListIt->nums.end()) {
          rowVals.pushBack(**iterIt);
          rowHasData = true;
          ++(*iterIt);
        }
        ++namedListIt;
        ++iterIt;
      }
      if (!rowHasData) {
        break;
      }
      bool firstInRow = true;
      for (NumCIter it = rowVals.cbegin(); it != rowVals.cend(); ++it) {
        if (!firstInRow) {
          std::cout << ' ';
        }
        std::cout << *it;
        firstInRow = false;
      }
      std::cout << '\n';
      size_t rowSum = 0;
      for (NumCIter it = rowVals.cbegin(); it != rowVals.cend(); ++it) {
        rowSum = checkedSum(rowSum, *it);
      }
      rowSums.pushBack(rowSum);
    }
  }

  static void printSums(const List< size_t >& sums)
  {
    if (sums.empty()) {
      std::cout << 0 << '\n';
      return;
    }
    using NumCIter = List< size_t >::const_iterator;
    bool first = true;
    for (NumCIter it = sums.cbegin(); it != sums.cend(); ++it) {
      if (!first) {
        std::cout << ' ';
      }
      std::cout << *it;
      first = false;
    }
    std::cout << '\n';
  }
}

int main()
{
  gordejchik::List< gordejchik::NamedList > namedLists;
  try {
    gordejchik::readInput(namedLists);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
  if (namedLists.empty()) {
    std::cout << 0 << '\n';
    return 0;
  }
  gordejchik::printNames(namedLists);
  gordejchik::List< size_t > rowSums;
  try {
    gordejchik::processZip(namedLists, rowSums);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
  gordejchik::printSums(rowSums);
  return 0;
}
