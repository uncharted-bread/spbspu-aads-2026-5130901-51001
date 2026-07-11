#ifndef MAP_HPP
#define MAP_HPP

#include <cstddef>
#include <functional>
#include <utility>
#include "avlTree.hpp"

namespace gordejchik {
  template< class Key, class Value, class Compare = std::less< Key > >
  class Map {
  public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = typename AVLTree< Key, Value, Compare >::value_type;
    using iterator = typename AVLTree< Key, Value, Compare >::iterator;
    using const_iterator = typename AVLTree< Key, Value, Compare >::const_iterator;
    using size_type = size_t;

    Value& operator[](const Key& k)
    {
      return tree_[k];
    }

    bool empty() const
    {
      return tree_.empty();
    }

    size_type size() const
    {
      return tree_.size();
    }

    void clear()
    {
      tree_.clear();
    }

    std::pair< iterator, bool > insert(const value_type& value)
    {
      return tree_.insert(value);
    }

    Value& at(const Key& k)
    {
      return tree_.get(k);
    }

    const Value& at(const Key& k) const
    {
      return tree_.get(k);
    }

    iterator find(const Key& k)
    {
      return tree_.find(k);
    }

    const_iterator find(const Key& k) const
    {
      return tree_.find(k);
    }

    size_type count(const Key& k) const
    {
      return tree_.contains(k) ? 1 : 0;
    }

    iterator erase(const_iterator pos)
    {
      return tree_.erase(pos);
    }

    size_type erase(const Key& k)
    {
      const_iterator pos = tree_.find(k);
      if (pos == tree_.cend()) {
        return 0;
      }
      tree_.erase(pos);
      return 1;
    }

    iterator begin()
    {
      return tree_.begin();
    }

    iterator end()
    {
      return tree_.end();
    }

    const_iterator begin() const
    {
      return tree_.cbegin();
    }

    const_iterator end() const
    {
      return tree_.cend();
    }

    const_iterator cbegin() const
    {
      return tree_.cbegin();
    }

    const_iterator cend() const
    {
      return tree_.cend();
    }

    void swap(Map& other)
    {
      tree_.swap(other.tree_);
    }

  private:
    AVLTree< Key, Value, Compare > tree_;
  };

  template< class Key, class Value, class Compare >
  void swap(Map< Key, Value, Compare >& a, Map< Key, Value, Compare >& b)
  {
    a.swap(b);
  }
}

#endif
