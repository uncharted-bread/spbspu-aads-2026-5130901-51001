#ifndef GORDEJCHIK_COMMANDS_HPP
#define GORDEJCHIK_COMMANDS_HPP

#include <iosfwd>
#include <string>
#include "graph.hpp"

namespace gordejchik {
  void cmdGraphs(const std::string& args, std::ostream& out,
      GraphCollection& graphs);
  void cmdVertexes(const std::string& args, std::ostream& out,
      GraphCollection& graphs);
  void cmdOutbound(const std::string& args, std::ostream& out,
      GraphCollection& graphs);
  void cmdInbound(const std::string& args, std::ostream& out,
      GraphCollection& graphs);
  void cmdBind(const std::string& args, std::ostream& out,
      GraphCollection& graphs);
  void cmdCut(const std::string& args, std::ostream& out,
      GraphCollection& graphs);
  void cmdCreate(const std::string& args, std::ostream& out,
      GraphCollection& graphs);
  void cmdMerge(const std::string& args, std::ostream& out,
      GraphCollection& graphs);
  void cmdExtract(const std::string& args, std::ostream& out,
      GraphCollection& graphs);
}

#endif
