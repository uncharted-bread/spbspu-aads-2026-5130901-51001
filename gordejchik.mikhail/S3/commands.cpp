#include "commands.hpp"
#include "safe-insert.hpp"
#include <iostream>
#include <algorithm>
#include <string>

static std::string extractWord(const std::string& line, size_t& pos)
{
  while (pos < line.size() && line[pos] == ' ') {
    ++pos;
  }
  size_t start = pos;
  while (pos < line.size() && line[pos] != ' ') {
    ++pos;
  }
  return line.substr(start, pos - start);
}

static void printInvalid(std::ostream& out)
{
  out << "<INVALID COMMAND>\n";
}

template< class Value, class Hash, class Equal >
static void printSortedKeys(
    const gordejchik::HashTable< std::string, Value, Hash, Equal >& table,
    std::ostream& out)
{
  using Table = gordejchik::HashTable< std::string, Value, Hash, Equal >;
  const size_t n = table.size();
  if (n == 0) {
    return;
  }
  std::string* names = new std::string[n];
  try {
    size_t idx = 0;
    for (typename Table::ConstIterator it = table.cbegin(); it != table.cend(); ++it) {
      names[idx++] = it->first;
    }
    std::sort(names, names + n);
    for (size_t i = 0; i < n; ++i) {
      out << names[i] << "\n";
    }
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

static void printNeighborLine(const gordejchik::Graph::WeightList& weights,
    const std::string& name, std::ostream& out)
{
  using WeightCIter = gordejchik::Graph::WeightList::const_iterator;
  const size_t count = weights.size();
  size_t* sorted = new size_t[count];
  size_t idx = 0;
  for (WeightCIter it = weights.cbegin(); it != weights.cend(); ++it) {
    sorted[idx++] = *it;
  }
  std::sort(sorted, sorted + count);
  out << name;
  for (size_t i = 0; i < count; ++i) {
    out << " " << sorted[i];
  }
  out << "\n";
  delete[] sorted;
}

static void printEdgeInfo(const gordejchik::Graph& g,
    const std::string& vertex, bool outbound, std::ostream& out)
{
  using EdgeCIter = gordejchik::Graph::EdgeTable::ConstIterator;
  using EdgeKey = gordejchik::Graph::EdgeKey;

  size_t neighborCount = 0;
  for (EdgeCIter it = g.edges().cbegin(); it != g.edges().cend(); ++it) {
    const std::string& check = outbound ? it->first.first : it->first.second;
    if (check == vertex) {
      ++neighborCount;
    }
  }
  if (neighborCount == 0) {
    return;
  }

  std::string* names = new std::string[neighborCount];
  try {
    size_t idx = 0;
    for (EdgeCIter it = g.edges().cbegin(); it != g.edges().cend(); ++it) {
      const std::string& check = outbound ? it->first.first : it->first.second;
      if (check == vertex) {
        names[idx++] = outbound ? it->first.second : it->first.first;
      }
    }
    std::sort(names, names + neighborCount);
    for (size_t i = 0; i < neighborCount; ++i) {
      const EdgeKey key = outbound
          ? EdgeKey(vertex, names[i])
          : EdgeKey(names[i], vertex);
      printNeighborLine(g.edges().at(key), names[i], out);
    }
  } catch (...) {
    delete[] names;
    throw;
  }
  delete[] names;
}

void gordejchik::cmdGraphs(const std::string&, std::ostream& out,
    GraphCollection& graphs)
{
  printSortedKeys(graphs, out);
}

void gordejchik::cmdVertexes(const std::string& args,
    std::ostream& out, GraphCollection& graphs)
{
  size_t pos = 0;
  std::string name = extractWord(args, pos);
  if (name.empty() || !graphs.contains(name)) {
    printInvalid(out);
    return;
  }
  printSortedKeys(graphs.at(name).vertices(), out);
}

void gordejchik::cmdOutbound(const std::string& args,
    std::ostream& out, GraphCollection& graphs)
{
  size_t pos = 0;
  std::string graphName = extractWord(args, pos);
  std::string vertex = extractWord(args, pos);
  if (graphName.empty() || vertex.empty()
      || !graphs.contains(graphName)) {
    printInvalid(out);
    return;
  }
  const Graph& g = graphs.at(graphName);
  if (!g.hasVertex(vertex)) {
    printInvalid(out);
    return;
  }
  printEdgeInfo(g, vertex, true, out);
}

void gordejchik::cmdInbound(const std::string& args,
    std::ostream& out, GraphCollection& graphs)
{
  size_t pos = 0;
  std::string graphName = extractWord(args, pos);
  std::string vertex = extractWord(args, pos);
  if (graphName.empty() || vertex.empty()
      || !graphs.contains(graphName)) {
    printInvalid(out);
    return;
  }
  const Graph& g = graphs.at(graphName);
  if (!g.hasVertex(vertex)) {
    printInvalid(out);
    return;
  }
  printEdgeInfo(g, vertex, false, out);
}

void gordejchik::cmdBind(const std::string& args,
    std::ostream& out, GraphCollection& graphs)
{
  size_t pos = 0;
  std::string graphName = extractWord(args, pos);
  std::string from = extractWord(args, pos);
  std::string to = extractWord(args, pos);
  if (graphName.empty() || from.empty() || to.empty()
      || !graphs.contains(graphName)) {
    printInvalid(out);
    return;
  }
  std::string wStr = extractWord(args, pos);
  if (wStr.empty()) {
    printInvalid(out);
    return;
  }
  size_t weight = 0;
  try {
    weight = std::stoul(wStr);
  } catch (...) {
    printInvalid(out);
    return;
  }
  graphs.at(graphName).addEdge(from, to, weight);
}

void gordejchik::cmdCut(const std::string& args,
    std::ostream& out, GraphCollection& graphs)
{
  size_t pos = 0;
  std::string graphName = extractWord(args, pos);
  std::string from = extractWord(args, pos);
  std::string to = extractWord(args, pos);
  if (graphName.empty() || from.empty() || to.empty()
      || !graphs.contains(graphName)) {
    printInvalid(out);
    return;
  }
  std::string wStr = extractWord(args, pos);
  if (wStr.empty()) {
    printInvalid(out);
    return;
  }
  size_t weight = 0;
  try {
    weight = std::stoul(wStr);
  } catch (...) {
    printInvalid(out);
    return;
  }
  Graph& g = graphs.at(graphName);
  if (!g.hasVertex(from) || !g.hasVertex(to)) {
    printInvalid(out);
    return;
  }
  if (!g.removeEdge(from, to, weight)) {
    printInvalid(out);
  }
}

void gordejchik::cmdCreate(const std::string& args,
    std::ostream& out, GraphCollection& graphs)
{
  if (args.empty()) {
    printInvalid(out);
    return;
  }
  size_t pos = 0;
  std::string name = extractWord(args, pos);
  if (name.empty() || graphs.contains(name)) {
    printInvalid(out);
    return;
  }
  Graph g;
  std::string countStr = extractWord(args, pos);
  if (!countStr.empty()) {
    size_t count = 0;
    try {
      count = std::stoul(countStr);
    } catch (...) {
      printInvalid(out);
      return;
    }
    for (size_t i = 0; i < count; ++i) {
      std::string v = extractWord(args, pos);
      if (v.empty()) {
        printInvalid(out);
        return;
      }
      g.addVertex(v);
    }
  }
  detail::safeInsert(graphs, name, g);
}

void gordejchik::cmdMerge(const std::string& args,
    std::ostream& out, GraphCollection& graphs)
{
  size_t pos = 0;
  std::string newName = extractWord(args, pos);
  std::string name1 = extractWord(args, pos);
  std::string name2 = extractWord(args, pos);
  if (newName.empty() || name1.empty() || name2.empty()) {
    printInvalid(out);
    return;
  }
  if (graphs.contains(newName)
      || !graphs.contains(name1) || !graphs.contains(name2)) {
    printInvalid(out);
    return;
  }
  const Graph& g1 = graphs.at(name1);
  const Graph& g2 = graphs.at(name2);
  Graph ng;

  using VCIter = Graph::VertexTable::ConstIterator;
  using ECIter = Graph::EdgeTable::ConstIterator;
  using WCIter = gordejchik::LCIter< size_t >;

  for (VCIter it = g1.vertices().cbegin();
      it != g1.vertices().cend(); ++it) {
    ng.addVertex(it->first);
  }
  for (VCIter it = g2.vertices().cbegin();
      it != g2.vertices().cend(); ++it) {
    ng.addVertex(it->first);
  }

  for (ECIter it = g1.edges().cbegin();
      it != g1.edges().cend(); ++it) {
    for (WCIter wit = it->second.cbegin();
        wit != it->second.cend(); ++wit) {
      ng.addEdge(it->first.first, it->first.second, *wit);
    }
  }
  for (ECIter it = g2.edges().cbegin();
      it != g2.edges().cend(); ++it) {
    for (WCIter wit = it->second.cbegin();
        wit != it->second.cend(); ++wit) {
      ng.addEdge(it->first.first, it->first.second, *wit);
    }
  }
  detail::safeInsert(graphs, newName, ng);
}

void gordejchik::cmdExtract(const std::string& args,
    std::ostream& out, GraphCollection& graphs)
{
  size_t pos = 0;
  std::string newName = extractWord(args, pos);
  std::string srcName = extractWord(args, pos);
  if (newName.empty() || srcName.empty()) {
    printInvalid(out);
    return;
  }
  if (graphs.contains(newName) || !graphs.contains(srcName)) {
    printInvalid(out);
    return;
  }

  std::string countStr = extractWord(args, pos);
  size_t count = 0;
  if (!countStr.empty()) {
    try {
      count = std::stoul(countStr);
    } catch (...) {
      printInvalid(out);
      return;
    }
  }

  const Graph& src = graphs.at(srcName);
  Graph ng;

  for (size_t i = 0; i < count; ++i) {
    std::string v = extractWord(args, pos);
    if (v.empty() || !src.hasVertex(v)) {
      printInvalid(out);
      return;
    }
    ng.addVertex(v);
  }

  using ECIter = Graph::EdgeTable::ConstIterator;
  using WCIter = gordejchik::LCIter< size_t >;

  for (ECIter it = src.edges().cbegin();
      it != src.edges().cend(); ++it) {
    const std::string& from = it->first.first;
    const std::string& to = it->first.second;
    if (ng.hasVertex(from) && ng.hasVertex(to)) {
      for (WCIter wit = it->second.cbegin();
          wit != it->second.cend(); ++wit) {
        ng.addEdge(from, to, *wit);
      }
    }
  }
  detail::safeInsert(graphs, newName, ng);
}
