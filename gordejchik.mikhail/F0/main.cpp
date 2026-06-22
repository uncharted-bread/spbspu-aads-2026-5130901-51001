#include <iostream>
#include <string>

int main()
{
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    std::cout << "<INVALID COMMAND>" << "\n";
  }
  return 0;
}
