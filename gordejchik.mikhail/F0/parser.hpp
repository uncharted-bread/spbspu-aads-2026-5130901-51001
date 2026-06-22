#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <cstddef>

namespace gordejchik {

  const size_t MAX_TOKENS = 10;

  struct ParsedCommand {
    std::string tokens_[MAX_TOKENS];
    size_t count_;
  };

  ParsedCommand parseLine(const std::string& line);

}

#endif
