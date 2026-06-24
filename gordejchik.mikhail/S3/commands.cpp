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

static size_t extractUnsigned(const std::string& line, size_t& pos)
{
  std::string word = extractWord(line, pos);
  return std::stoul(word);
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

void gordejchik::cmdGraphs(const std::string&, std::ostream& out,
    GraphCollection& graphs)
{
  const size_t n = graphs.size();
  if (n == 0) {
    return;
  }
  std::string* names = new std::string[n];
  size_t idx = 0;
  for (auto it = graphs.cbegin(); it != graphs.cend(); ++it) {
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
  std::string* verts = new std::string[n];
  size_t idx = 0;
  for (auto it = g.vertices().cbegin();
      it != g.vertices().cend(); ++it) {
    verts[idx++] = it->first;
  }
  std::sort(verts, verts + n);
  for (size_t i = 0; i < n; ++i) {
    out << verts[i] << "\n";
  }
  delete[] verts;
}

static void printEdgeInfo(const gordejchik::Graph& g,
    const std::string& vertex, bool outbound, std::ostream& out)
{
  using EdgeTable = gordejchik::Graph::EdgeTable;

  size_t pairCount = 0;
  for (auto it = g.edges().cbegin(); it != g.edges().cend(); ++it) {
    const std::string& check = outbound ? it->first.first : it->first.second;
    if (check == vertex) {
      ++pairCount;
    }
  }
  if (pairCount == 0) {
    return;
  }

  struct NeighborInfo {
    std::string name;
    size_t* weights;
    size_t count;
  };

  NeighborInfo* neighbors = new NeighborInfo[pairCount];
  size_t idx = 0;

  for (auto it = g.edges().cbegin(); it != g.edges().cend(); ++it) {
    const std::string& check = outbound ? it->first.first : it->first.second;
    if (check != vertex) {
      continue;
    }
    const std::string& other = outbound ? it->first.second : it->first.first;
    const gordejchik::Graph::WeightList& wl = it->second;

    size_t wCount = 0;
    for (auto wit = wl.cbegin(); wit != wl.cend(); ++wit) {
      ++wCount;
    }

    size_t* warr = new size_t[wCount];
    size_t wi = 0;
    for (auto wit = wl.cbegin(); wit != wl.cend(); ++wit) {
      warr[wi++] = *wit;
    }
    std::sort(warr, warr + wCount);

    neighbors[idx].name = other;
    neighbors[idx].weights = warr;
    neighbors[idx].count = wCount;
    ++idx;
  }

  std::sort(neighbors, neighbors + pairCount,
      [](const NeighborInfo& a, const NeighborInfo& b) {
        return a.name < b.name;
      });

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
