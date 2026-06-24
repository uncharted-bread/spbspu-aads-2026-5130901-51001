#ifndef GRAPH_H
#define GRAPH_H

#include <cstddef>
#include <string>
#include <utility>
#include "hash-table.hpp"
#include "sha1.hpp"
#include "list.hpp"

namespace gordejchik {
  class Graph {
  public:
    using EdgeKey = std::pair< std::string, std::string >;
    using WeightList = List< size_t >;
    using VertexTable = HashTable< std::string, char,
        Sha1Hash< std::string >, StringEqual >;
    using EdgeTable = HashTable< EdgeKey, WeightList,
        Sha1Hash< EdgeKey >, PairStringEqual >;

    Graph();

    void addVertex(const std::string& name);
    bool hasVertex(const std::string& name) const;

    void addEdge(const std::string& from,
        const std::string& to, size_t weight);
    bool removeEdge(const std::string& from,
        const std::string& to, size_t weight);

    VertexTable& vertices();
    const VertexTable& vertices() const;
    EdgeTable& edges();
    const EdgeTable& edges() const;

  private:
    VertexTable vertices_;
    EdgeTable edges_;
  };

  using GraphCollection = HashTable< std::string, Graph,
      Sha1Hash< std::string >, StringEqual >;

  void readGraphs(const std::string& filename, GraphCollection& graphs);
}

#endif
