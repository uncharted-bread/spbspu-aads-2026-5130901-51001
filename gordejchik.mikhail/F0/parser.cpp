#include "parser.hpp"

gordejchik::ParsedCommand gordejchik::parseLine(
    const std::string& line)
{
  ParsedCommand result;
  result.count_ = 0;
  size_t i = 0;
  const size_t len = line.size();
  while (i < len && result.count_ < MAX_TOKENS) {
    while (i < len && line[i] == ' ') {
      ++i;
    }
    if (i >= len) {
      break;
    }
    if (line[i] == '"') {
      ++i;
      std::string token;
      while (i < len && line[i] != '"') {
        token += line[i];
        ++i;
      }
      if (i < len) {
        ++i;
      }
      result.tokens_[result.count_] = token;
      ++result.count_;
    } else {
      std::string token;
      while (i < len && line[i] != ' ') {
        token += line[i];
        ++i;
      }
      result.tokens_[result.count_] = token;
      ++result.count_;
    }
  }
  return result;
}
