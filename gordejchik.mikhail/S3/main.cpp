#include <cstddef>
#include <iostream>
#include <string>
#include "hash-table.hpp"
#include "sha1.hpp"
#include "graph.hpp"
#include "commands.hpp"

int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "Usage: lab <filename>\n";
    return 1;
  }

  const size_t graphBuckets = 8;
  const size_t commandBuckets = 16;
  const size_t bucketSize = 4;
  gordejchik::GraphCollection graphs(graphBuckets, bucketSize);

  try {
    gordejchik::readGraphs(argv[1], graphs);
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  using CmdFunc = void(*)(const std::string&, std::ostream&,
      gordejchik::GraphCollection&);
  using CmdTable = gordejchik::HashTable< std::string, CmdFunc,
      gordejchik::Sha1Hash< std::string >, gordejchik::StringEqual >;

  CmdTable cmds(commandBuckets, bucketSize);
  cmds.insert("graphs", &gordejchik::cmdGraphs);
  cmds.insert("vertexes", &gordejchik::cmdVertexes);
  cmds.insert("outbound", &gordejchik::cmdOutbound);
  cmds.insert("inbound", &gordejchik::cmdInbound);
  cmds.insert("bind", &gordejchik::cmdBind);
  cmds.insert("cut", &gordejchik::cmdCut);
  cmds.insert("create", &gordejchik::cmdCreate);
  cmds.insert("merge", &gordejchik::cmdMerge);
  cmds.insert("extract", &gordejchik::cmdExtract);

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    size_t sp = line.find(' ');
    std::string cmd = (sp == std::string::npos)
        ? line
        : line.substr(0, sp);
    std::string rest = (sp == std::string::npos)
        ? ""
        : line.substr(sp + 1);

    if (cmds.contains(cmd)) {
      try {
        cmds.at(cmd)(rest, std::cout, graphs);
      } catch (const std::exception& e) {
        std::cerr << "Internal error: " << e.what() << "\n";
        return 2;
      }
    } else {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
  return 0;
}
