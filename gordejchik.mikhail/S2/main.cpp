#include <fstream>
#include <iostream>
#include <string>
#include "calc.hpp"
#include "stack.hpp"

int main(int argc, char* argv[])
{
  std::ifstream file;
  if (argc > 1) {
    file.open(argv[1]);
    if (!file.is_open()) {
      std::cerr << "Cannot open file: " << argv[1] << "\n";
      return 1;
    }
  }
  std::istream& input = (argc > 1) ? file : std::cin;

  gordejchik::Stack< long long > results;
  std::string line;
  while (std::getline(input, line)) {
    if (line.find_first_not_of(' ') == std::string::npos) {
      continue;
    }
    try {
      const long long result = gordejchik::calculateExpression(line);
      results.push(result);
    } catch (const std::exception& e) {
      std::cerr << e.what() << "\n";
      return 1;
    }
  }

  if (!results.empty()) {
    std::cout << results.top();
    results.pop();
    while (!results.empty()) {
      std::cout << " " << results.top();
      results.pop();
    }
    std::cout << "\n";
  }
  return 0;
}
