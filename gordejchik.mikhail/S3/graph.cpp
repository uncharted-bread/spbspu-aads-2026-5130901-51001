#include "graph.hpp"
#include <fstream>
#include <stdexcept>
#include "safe-insert.hpp"

static const size_t initialVertexBuckets = 8;
static const size_t initialEdgeBuckets = 16;
static const size_t initialBucketSize = 4;

gordejchik::Graph::Graph():
  vertices_(initialVertexBuckets, initialBucketSize),
  edges_(initialEdgeBuckets, initialBucketSize)
{}

void gordejchik::Graph::addVertex(const std::string& name)
{
  if (vertices_.contains(name)) {
    return;
  }
  detail::safeInsert(vertices_, name, 'v');
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
    detail::safeInsert(edges_, key, wl);
  }
}

bool gordejchik::Graph::removeEdge(const std::string& from,
    const std::string& to, size_t weight)
{
  EdgeKey key(from, to);
  if (!edges_.contains(key)) {
    return false;
  }
  WeightList& weights = edges_.at(key);
  bool removed = false;
  for (WeightList::iterator it = weights.begin(); it != weights.end(); ++it) {
    if (*it == weight) {
      weights.erase(it);
      removed = true;
      break;
    }
  }
  if (!removed) {
    return false;
  }
  if (weights.empty()) {
    edges_.erase(key);
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
    throw std::runtime_error("Cannot open file: " + filename);
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
    detail::safeInsert(graphs, graphName, g);
  }
}
