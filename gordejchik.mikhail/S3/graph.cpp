#include "graph.hpp"
#include <fstream>
#include <stdexcept>

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

gordejchik::Graph::Graph():
  vertices_(8, 4),
  edges_(16, 4)
{}

void gordejchik::Graph::addVertex(const std::string& name)
{
  if (vertices_.contains(name)) {
    return;
  }
  safeInsert(vertices_, name, 'v');
}

bool gordejchik::Graph::hasVertex(const std::string& name) const
{
  return vertices_.contains(name);
}

void gordejchik::Graph::addEdge(const std::string& from,
    const std::string& to, size_t weight)
{
  addVertex(from);
  addVertex(to);
  EdgeKey key(from, to);
  if (edges_.contains(key)) {
    edges_.at(key).pushBack(weight);
  } else {
    WeightList wl;
    wl.pushBack(weight);
    safeInsert(edges_, key, wl);
  }
}

bool gordejchik::Graph::removeEdge(const std::string& from,
    const std::string& to, size_t weight)
{
  EdgeKey key(from, to);
  if (!edges_.contains(key)) {
    return false;
  }
  WeightList& wl = edges_.at(key);
  WeightList newList;
  bool removed = false;
  for (auto it = wl.cbegin(); it != wl.cend(); ++it) {
    if (!removed && *it == weight) {
      removed = true;
    } else {
      newList.pushBack(*it);
    }
  }
  if (!removed) {
    return false;
  }
  if (newList.empty()) {
    edges_.erase(key);
  } else {
    wl = newList;
  }
  return true;
}

gordejchik::Graph::VertexTable& gordejchik::Graph::vertices()
{
  return vertices_;
}

const gordejchik::Graph::VertexTable& gordejchik::Graph::vertices() const
{
  return vertices_;
}

gordejchik::Graph::EdgeTable& gordejchik::Graph::edges()
{
  return edges_;
}

const gordejchik::Graph::EdgeTable& gordejchik::Graph::edges() const
{
  return edges_;
}

void gordejchik::readGraphs(const std::string& filename,
    GraphCollection& graphs)
{
  std::ifstream file(filename);
  if (!file) {
    throw std::runtime_error("Не получается открыть файл: " + filename);
  }
  std::string graphName;
  size_t edgeCount = 0;
  while (file >> graphName >> edgeCount) {
    Graph g;
    for (size_t i = 0; i < edgeCount;) {
      std::string from;
      std::string to;
      size_t weight = 0;
      if (!(file >> from >> to >> weight)) {
        break;
      }
      g.addEdge(from, to, weight);
      ++i;
    }
    safeInsert(graphs, graphName, g);
  }
}
