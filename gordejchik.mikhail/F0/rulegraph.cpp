#include "rulegraph.hpp"
#include <string>
#include <utility>

static std::pair< std::string, std::string > splitRule(const std::string& key)
{
  const size_t sep = key.find('>');
  return std::make_pair(key.substr(0, sep), key.substr(sep + 1));
}

gordejchik::RuleGraph::RuleGraph(const GameRules& rules):
  indexOf_(),
  names_(nullptr),
  vertexCount_(0),
  edgeCount_(0),
  offsets_(nullptr),
  targets_(nullptr)
{
  std::pair< size_t, size_t >* edges = nullptr;
  size_t* cursors = nullptr;
  try {
    for (GameRules::ConstIterator it = rules.cbegin(); it != rules.cend(); ++it) {
      if (it->first.find('>') == std::string::npos) {
        continue;
      }
      const std::pair< std::string, std::string > rule = splitRule(it->first);
      if (!indexOf_.contains(rule.first)) {
        indexOf_.insert(rule.first, vertexCount_);
        ++vertexCount_;
      }
      if (!indexOf_.contains(rule.second)) {
        indexOf_.insert(rule.second, vertexCount_);
        ++vertexCount_;
      }
      ++edgeCount_;
    }
    names_ = new std::string[vertexCount_];
    using IndexIterator = HashTable< std::string, size_t >::ConstIterator;
    for (IndexIterator it = indexOf_.cbegin(); it != indexOf_.cend(); ++it) {
      names_[it->second] = it->first;
    }
    edges = new std::pair< size_t, size_t >[edgeCount_];
    size_t edgeIndex = 0;
    for (GameRules::ConstIterator it = rules.cbegin(); it != rules.cend(); ++it) {
      if (it->first.find('>') == std::string::npos) {
        continue;
      }
      const std::pair< std::string, std::string > rule = splitRule(it->first);
      edges[edgeIndex].first = indexOf_.at(rule.first);
      edges[edgeIndex].second = indexOf_.at(rule.second);
      ++edgeIndex;
    }
    offsets_ = new size_t[vertexCount_ + 1]();
    for (size_t e = 0; e < edgeCount_; ++e) {
      ++offsets_[edges[e].first + 1];
    }
    for (size_t v = 1; v <= vertexCount_; ++v) {
      offsets_[v] += offsets_[v - 1];
    }
    targets_ = new size_t[edgeCount_];
    cursors = new size_t[vertexCount_]();
    for (size_t e = 0; e < edgeCount_; ++e) {
      const size_t from = edges[e].first;
      targets_[offsets_[from] + cursors[from]] = edges[e].second;
      ++cursors[from];
    }
  } catch (...) {
    delete[] edges;
    delete[] cursors;
    delete[] names_;
    delete[] offsets_;
    delete[] targets_;
    throw;
  }
  delete[] edges;
  delete[] cursors;
}

gordejchik::RuleGraph::~RuleGraph()
{
  delete[] names_;
  delete[] offsets_;
  delete[] targets_;
}

size_t gordejchik::RuleGraph::vertexCount() const
{
  return vertexCount_;
}

const std::string& gordejchik::RuleGraph::vertexName(size_t index) const
{
  return names_[index];
}

size_t gordejchik::RuleGraph::findCycle(size_t* cycle) const
{
  if (vertexCount_ == 0) {
    return 0;
  }
  char* colors = new char[vertexCount_]();
  size_t* path = nullptr;
  size_t length = 0;
  try {
    path = new size_t[vertexCount_];
    for (size_t v = 0; v < vertexCount_ && length == 0; ++v) {
      if (colors[v] == 0) {
        length = dfsCycle(v, colors, path, 0, cycle);
      }
    }
  } catch (...) {
    delete[] colors;
    delete[] path;
    throw;
  }
  delete[] colors;
  delete[] path;
  return length;
}

size_t gordejchik::RuleGraph::dfsCycle(size_t vertex, char* colors, size_t* path,
    size_t depth, size_t* cycle) const
{
  colors[vertex] = 1;
  path[depth] = vertex;
  for (size_t e = offsets_[vertex]; e < offsets_[vertex + 1]; ++e) {
    const size_t to = targets_[e];
    if (colors[to] == 1) {
      size_t start = depth;
      while (path[start] != to) {
        --start;
      }
      size_t length = 0;
      for (size_t i = start; i <= depth; ++i) {
        cycle[length] = path[i];
        ++length;
      }
      return length;
    }
    if (colors[to] == 0) {
      const size_t found = dfsCycle(to, colors, path, depth + 1, cycle);
      if (found > 0) {
        return found;
      }
    }
  }
  colors[vertex] = 2;
  return 0;
}
