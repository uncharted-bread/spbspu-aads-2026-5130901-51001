#ifndef GORDEJCHIK_SAFE_INSERT_HPP
#define GORDEJCHIK_SAFE_INSERT_HPP

#include <cstddef>
#include <stdexcept>
#include "hash-table.hpp"

namespace gordejchik {
  namespace detail {
    template< class Key, class Value, class Hash, class Equal >
    void safeInsert(HashTable< Key, Value, Hash, Equal >& table,
        const Key& key, const Value& value)
    {
      const size_t maxAttempts = 5;
      for (size_t attempt = 0; attempt < maxAttempts; ++attempt) {
        try {
          table.insert(key, value);
          return;
        } catch (const std::overflow_error&) {
          table.rehash(table.bucketCount() * 2);
        }
      }
      table.insert(key, value);
    }
  }
}

#endif
