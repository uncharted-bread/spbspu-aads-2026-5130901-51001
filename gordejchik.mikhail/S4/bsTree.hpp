#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <cstddef>
#include <utility>
#include <functional>
#include <stdexcept>

namespace gordejchik {
  namespace detail {
    template< class Key, class Value >
    struct BSTNode {
      std::pair< Key, Value > data_;
      BSTNode* left_;
      BSTNode* right_;
      BSTNode* parent_;

      BSTNode(const Key& k, const Value& v, BSTNode* parent):
        data_(k, v),
        left_(nullptr),
        right_(nullptr),
        parent_(parent)
      {}
    };
  }

  template< class Key, class Value > class BSTIterator;
  template< class Key, class Value > class BSTConstIterator;

  template< class Key, class Value, class Compare = std::less< Key > >
  class BSTree {
  public:
    using iterator = BSTIterator< Key, Value >;
    using const_iterator = BSTConstIterator< Key, Value >;
    using value_type = std::pair< Key, Value >;

    BSTree():
      root_(nullptr),
      size_(0),
      cmp_()
    {}

    ~BSTree()
    {
      clear();
    }

    bool empty() const
    {
      return size_ == 0;
    }

    std::size_t size() const
    {
      return size_;
    }

    void clear()
    {
      freeSubtree(root_);
      root_ = nullptr;
      size_ = 0;
    }

  private:
    using Node_ = detail::BSTNode< Key, Value >;

    Node_* root_;
    std::size_t size_;
    Compare cmp_;

    static void freeSubtree(Node_* node)
    {
      if (!node) {
        return;
      }
      freeSubtree(node->left_);
      freeSubtree(node->right_);
      delete node;
    }

    template< class K, class V >
    friend class BSTIterator;
    template< class K, class V >
    friend class BSTConstIterator;
  };
}

#endif
