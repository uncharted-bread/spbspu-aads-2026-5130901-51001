#ifndef RULEGRAPH_HPP
#define RULEGRAPH_HPP

#include <cstddef>
#include <string>
#include "gameconfig.hpp"
#include "hashtable.hpp"

namespace gordejchik {

  class RuleGraph {
  public:
    RuleGraph(const RuleGraph&) = delete;
    explicit RuleGraph(const GameRules& rules);
    ~RuleGraph();

    RuleGraph& operator=(const RuleGraph&) = delete;

    size_t vertexCount() const;
    const std::string& vertexName(size_t index) const;
    // The cycle buffer must have room for vertexCount() indices;
    // returns the cycle length, 0 when the graph is acyclic.
    size_t findCycle(size_t* cycle) const;

  private:
    HashTable< std::string, size_t > indexOf_;
    std::string* names_;
    size_t vertexCount_;
    size_t edgeCount_;
    size_t* offsets_;
    size_t* targets_;

    size_t dfsCycle(size_t vertex, char* colors, size_t* path,
        size_t depth, size_t* cycle) const;
  };
}

#endif
