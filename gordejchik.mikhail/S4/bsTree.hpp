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

    void push(const Key& k, const Value& v)
    {
      if (!root_) {
        root_ = new Node_(k, v, nullptr);
        ++size_;
        return;
      }
      Node_* cur = root_;
      while (cur) {
        if (cmp_(k, cur->data_.first)) {
          if (cur->left_) {
            cur = cur->left_;
          } else {
            cur->left_ = new Node_(k, v, cur);
            ++size_;
            return;
          }
        } else if (cmp_(cur->data_.first, k)) {
          if (cur->right_) {
            cur = cur->right_;
          } else {
            cur->right_ = new Node_(k, v, cur);
            ++size_;
            return;
          }
        } else {
          cur->data_.second = v;
          return;
        }
      }
    }

    const Value& get(const Key& k) const
    {
      const Node_* node = findNode(k);
      if (!node) {
        throw std::out_of_range("BSTree: ключ не найден");
      }
      return node->data_.second;
    }

    Value& get(const Key& k)
    {
      Node_* node = findNode(k);
      if (!node) {
        throw std::out_of_range("BSTree: ключ не найден");
      }
      return node->data_.second;
    }

    bool contains(const Key& k) const
    {
      return findNode(k) != nullptr;
    }

    Value& operator[](const Key& k)
    {
      Node_* node = findNode(k);
      if (node) {
        return node->data_.second;
      }
      push(k, Value());
      return findNode(k)->data_.second;
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

    Node_* findNode(const Key& k) const
    {
      Node_* cur = root_;
      while (cur) {
        if (cmp_(k, cur->data_.first)) {
          cur = cur->left_;
        } else if (cmp_(cur->data_.first, k)) {
          cur = cur->right_;
        } else {
          return cur;
        }
      }
      return nullptr;
    }

    template< class K, class V >
    friend class BSTIterator;
    template< class K, class V >
    friend class BSTConstIterator;
  };
}

#endif
