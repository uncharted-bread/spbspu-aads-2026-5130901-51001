#ifndef AVLTREE_HPP
#define AVLTREE_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include "bsTree.hpp"

namespace gordejchik {
  template< class Key, class Value, class Compare = std::less< Key > >
  class AVLTree: private BSTree< Key, Value, Compare > {
  public:
    using iterator = typename BSTree< Key, Value, Compare >::iterator;
    using const_iterator = typename BSTree< Key, Value, Compare >::const_iterator;
    using value_type = typename BSTree< Key, Value, Compare >::value_type;

    using BSTree< Key, Value, Compare >::empty;
    using BSTree< Key, Value, Compare >::size;
    using BSTree< Key, Value, Compare >::clear;
    using BSTree< Key, Value, Compare >::get;
    using BSTree< Key, Value, Compare >::contains;
    using BSTree< Key, Value, Compare >::find;
    using BSTree< Key, Value, Compare >::begin;
    using BSTree< Key, Value, Compare >::end;
    using BSTree< Key, Value, Compare >::cbegin;
    using BSTree< Key, Value, Compare >::cend;
    using BSTree< Key, Value, Compare >::height;

    Value& operator[](const Key& k)
    {
      bool inserted = false;
      Node* node = this->insertNode(k, Value(), inserted);
      if (inserted) {
        rebalanceFrom(node->parent_);
      }
      return node->data_.second;
    }

    void push(const Key& k, const Value& v)
    {
      bool inserted = false;
      Node* node = this->insertNode(k, v, inserted);
      if (inserted) {
        rebalanceFrom(node->parent_);
      } else {
        node->data_.second = v;
      }
    }

    std::pair< iterator, bool > insert(const value_type& value)
    {
      bool inserted = false;
      Node* node = this->insertNode(value.first, value.second, inserted);
      if (inserted) {
        rebalanceFrom(node->parent_);
      }
      return std::pair< iterator, bool >(this->makeIterator(node), inserted);
    }

    iterator erase(const_iterator pos)
    {
      Node* node = this->nodeOf(pos);
      if (!node) {
        return this->end();
      }
      Node* next = detail::nextInOrder< Key, Value >(node);
      Node* fixup = this->eraseNode(node);
      rebalanceFrom(fixup);
      return this->makeIterator(next);
    }

    Value drop(const Key& k)
    {
      Node* node = this->findNode(k);
      if (!node) {
        throw std::out_of_range("AVLTree: key not found");
      }
      Value result = node->data_.second;
      Node* fixup = this->eraseNode(node);
      rebalanceFrom(fixup);
      return result;
    }

    void swap(AVLTree& other)
    {
      BSTree< Key, Value, Compare >::swap(other);
    }

  private:
    using Node = typename BSTree< Key, Value, Compare >::Node;

    void rebalanceFrom(Node* node)
    {
      Node* cur = node;
      while (cur) {
        this->updateHeight(cur);
        const std::ptrdiff_t balance = balanceOf(cur);
        if (balance > 1) {
          if (balanceOf(cur->left_) >= 0) {
            cur = this->rotateRightNode(cur->left_);
          } else {
            this->rotateLeftNode(cur->left_->right_);
            cur = this->rotateRightNode(cur->left_);
          }
        } else if (balance < -1) {
          if (balanceOf(cur->right_) <= 0) {
            cur = this->rotateLeftNode(cur->right_);
          } else {
            this->rotateRightNode(cur->right_->left_);
            cur = this->rotateLeftNode(cur->right_);
          }
        }
        cur = cur->parent_;
      }
    }

    static std::ptrdiff_t balanceOf(const Node* node)
    {
      const size_t lh = BSTree< Key, Value, Compare >::nodeHeight(node->left_);
      const size_t rh = BSTree< Key, Value, Compare >::nodeHeight(node->right_);
      return static_cast< std::ptrdiff_t >(lh) - static_cast< std::ptrdiff_t >(rh);
    }
  };

  template< class Key, class Value, class Compare >
  void swap(AVLTree< Key, Value, Compare >& a, AVLTree< Key, Value, Compare >& b)
  {
    a.swap(b);
  }
}

#endif
