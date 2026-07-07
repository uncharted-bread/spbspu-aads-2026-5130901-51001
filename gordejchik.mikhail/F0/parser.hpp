#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstddef>
#include <string>

namespace gordejchik {

  const size_t MAX_TOKENS = 10;

  struct ParsedCommand {
    std::string tokens_[MAX_TOKENS];
    size_t count_;
  };

  ParsedCommand parseLine(const std::string& line);

}

#endif
