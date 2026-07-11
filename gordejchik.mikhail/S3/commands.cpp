#include "commands.hpp"
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

template< class K, class V, class H, class E >
static void safeInsert(gordejchik::HashTable< K, V, H, E >& table,
    const K& key, const V& val)
{
  for (size_t attempt = 0; attempt < 5; ++attempt) {
    try {
      table.insert(key, val);
      return;
    } catch (const std::overflow_error&) {
      table.rehash(table.bucketCount() * 2);
    }
  }
  table.insert(key, val);
}

struct NeighborInfo {
  std::string name;
  size_t* weights;
  size_t count;
};

static bool compareNeighbors(const NeighborInfo& a, const NeighborInfo& b)
{
  return a.name < b.name;
}

static void printEdgeInfo(const gordejchik::Graph& g,
    const std::string& vertex, bool outbound, std::ostream& out)
{
  using EdgeCIter = gordejchik::Graph::EdgeTable::ConstIterator;
  using WCIter = gordejchik::LCIter< size_t >;

  size_t pairCount = 0;
  for (EdgeCIter it = g.edges().cbegin();
      it != g.edges().cend(); ++it) {
    const std::string& check = outbound
        ? it->first.first
        : it->first.second;
    if (check == vertex) {
      ++pairCount;
    }
  }
  if (pairCount == 0) {
    return;
  }

  NeighborInfo* neighbors = new NeighborInfo[pairCount];
  size_t idx = 0;

  for (EdgeCIter it = g.edges().cbegin();
      it != g.edges().cend(); ++it) {
    const std::string& check = outbound
        ? it->first.first
        : it->first.second;
    if (check != vertex) {
      continue;
    }
    const std::string& other = outbound
        ? it->first.second
        : it->first.first;
    const gordejchik::Graph::WeightList& wl = it->second;

    size_t wCount = 0;
    for (WCIter wit = wl.cbegin(); wit != wl.cend(); ++wit) {
      ++wCount;
    }

    size_t* warr = new size_t[wCount];
    size_t wi = 0;
    for (WCIter wit = wl.cbegin(); wit != wl.cend(); ++wit) {
      warr[wi++] = *wit;
    }
    std::sort(warr, warr + wCount);

    neighbors[idx].name = other;
    neighbors[idx].weights = warr;
    neighbors[idx].count = wCount;
    ++idx;
  }

  std::sort(neighbors, neighbors + pairCount, compareNeighbors);

  for (size_t i = 0; i < pairCount; ++i) {
    out << neighbors[i].name;
    for (size_t j = 0; j < neighbors[i].count; ++j) {
      out << " " << neighbors[i].weights[j];
    }
    out << "\n";
    delete[] neighbors[i].weights;
  }
  delete[] neighbors;
}

void gordejchik::cmdGraphs(const std::string&, std::ostream& out,
    GraphCollection& graphs)
{
  const size_t n = graphs.size();
  if (n == 0) {
    return;
  }
  using CIter = GraphCollection::ConstIterator;
  std::string* names = new std::string[n];
  size_t idx = 0;
  for (CIter it = graphs.cbegin(); it != graphs.cend(); ++it) {
    names[idx++] = it->first;
  }
  std::sort(names, names + n);
  for (size_t i = 0; i < n; ++i) {
    out << names[i] << "\n";
  }
  delete[] names;
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
  const Graph& g = graphs.at(name);
  const size_t n = g.vertices().size();
  if (n == 0) {
    return;
  }
  using VCIter = Graph::VertexTable::ConstIterator;
  std::string* verts = new std::string[n];
  size_t idx = 0;
  for (VCIter it = g.vertices().cbegin();
      it != g.vertices().cend(); ++it) {
    verts[idx++] = it->first;
  }
  std::sort(verts, verts + n);
  for (size_t i = 0; i < n; ++i) {
    out << verts[i] << "\n";
  }
  delete[] verts;
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
  safeInsert(graphs, name, g);
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
  safeInsert(graphs, newName, ng);
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
  safeInsert(graphs, newName, ng);
}
