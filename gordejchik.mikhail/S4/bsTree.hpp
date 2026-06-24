#ifndef BSTREE_HPP
#define BSTREE_HPP

#include <cstddef>
#include <utility>
#include <functional>
#include <stdexcept>
#include <iterator>

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

    template< class Key, class Value >
    BSTNode< Key, Value >* leftmost(BSTNode< Key, Value >* node)
    {
      if (!node) {
        return nullptr;
      }
      while (node->left_) {
        node = node->left_;
      }
      return node;
    }

    template< class Key, class Value >
    BSTNode< Key, Value >* rightmost(BSTNode< Key, Value >* node)
    {
      if (!node) {
        return nullptr;
      }
      while (node->right_) {
        node = node->right_;
      }
      return node;
    }

    template< class Key, class Value >
    BSTNode< Key, Value >* nextInOrder(BSTNode< Key, Value >* node)
    {
      if (node->right_) {
        return leftmost< Key, Value >(node->right_);
      }
      BSTNode< Key, Value >* parent = node->parent_;
      while (parent && node == parent->right_) {
        node = parent;
        parent = parent->parent_;
      }
      return parent;
    }

    template< class Key, class Value >
    BSTNode< Key, Value >* prevInOrder(BSTNode< Key, Value >* node)
    {
      if (node->left_) {
        return rightmost< Key, Value >(node->left_);
      }
      BSTNode< Key, Value >* parent = node->parent_;
      while (parent && node == parent->left_) {
        node = parent;
        parent = parent->parent_;
      }
      return parent;
    }
  }

  template< class Key, class Value > class BSTConstIterator;

  template< class Key, class Value >
  class BSTIterator {
    template< class K, class V, class C >
    friend class BSTree;
    friend class BSTConstIterator< Key, Value >;

  public:
    using Node_ = detail::BSTNode< Key, Value >;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair< Key, Value >;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    BSTIterator():
      node_(nullptr),
      rootAddr_(nullptr)
    {}

    reference operator*() const
    {
      return node_->data_;
    }

    pointer operator->() const
    {
      return &node_->data_;
    }

    BSTIterator& operator++()
    {
      node_ = detail::nextInOrder< Key, Value >(node_);
      return *this;
    }

    BSTIterator operator++(int)
    {
      BSTIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    BSTIterator& operator--()
    {
      if (!node_) {
        node_ = detail::rightmost< Key, Value >(*rootAddr_);
      } else {
        node_ = detail::prevInOrder< Key, Value >(node_);
      }
      return *this;
    }

    BSTIterator operator--(int)
    {
      BSTIterator tmp = *this;
      --(*this);
      return tmp;
    }

    bool operator==(const BSTIterator& rhs) const
    {
      return node_ == rhs.node_;
    }

    bool operator!=(const BSTIterator& rhs) const
    {
      return node_ != rhs.node_;
    }

  private:
    Node_* node_;
    Node_** rootAddr_;

    BSTIterator(Node_* node, Node_** rootAddr):
      node_(node),
      rootAddr_(rootAddr)
    {}
  };

  template< class Key, class Value >
  class BSTConstIterator {
    template< class K, class V, class C >
    friend class BSTree;

  public:
    using Node_ = detail::BSTNode< Key, Value >;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::pair< Key, Value >;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    BSTConstIterator():
      node_(nullptr),
      rootAddr_(nullptr)
    {}

    BSTConstIterator(const BSTIterator< Key, Value >& other):
      node_(other.node_),
      rootAddr_(other.rootAddr_)
    {}

    reference operator*() const
    {
      return node_->data_;
    }

    pointer operator->() const
    {
      return &node_->data_;
    }

    BSTConstIterator& operator++()
    {
      node_ = detail::nextInOrder< Key, Value >(node_);
      return *this;
    }

    BSTConstIterator operator++(int)
    {
      BSTConstIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    BSTConstIterator& operator--()
    {
      if (!node_) {
        node_ = detail::rightmost< Key, Value >(*rootAddr_);
      } else {
        node_ = detail::prevInOrder< Key, Value >(node_);
      }
      return *this;
    }

    BSTConstIterator operator--(int)
    {
      BSTConstIterator tmp = *this;
      --(*this);
      return tmp;
    }

    bool operator==(const BSTConstIterator& rhs) const
    {
      return node_ == rhs.node_;
    }

    bool operator!=(const BSTConstIterator& rhs) const
    {
      return node_ != rhs.node_;
    }

  private:
    Node_* node_;
    Node_** rootAddr_;

    BSTConstIterator(Node_* node, Node_** rootAddr):
      node_(node),
      rootAddr_(rootAddr)
    {}
  };

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

    size_t size() const
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

    iterator begin()
    {
      return iterator(detail::leftmost< Key, Value >(root_), &root_);
    }

    iterator end()
    {
      return iterator(nullptr, &root_);
    }

    const_iterator begin() const
    {
      return cbegin();
    }

    const_iterator end() const
    {
      return cend();
    }

    const_iterator cbegin() const
    {
      Node_** addr = const_cast< Node_** >(&root_);
      return const_iterator(detail::leftmost< Key, Value >(root_), addr);
    }

    const_iterator cend() const
    {
      Node_** addr = const_cast< Node_** >(&root_);
      return const_iterator(nullptr, addr);
    }

    iterator find(const Key& k)
    {
      return iterator(findNode(k), &root_);
    }

    const_iterator find(const Key& k) const
    {
      Node_** addr = const_cast< Node_** >(&root_);
      return const_iterator(findNode(k), addr);
    }

    iterator erase(const_iterator pos)
    {
      Node_* node = pos.node_;
      if (!node) {
        return end();
      }
      Node_* next = detail::nextInOrder< Key, Value >(node);
      eraseNode(node);
      --size_;
      return iterator(next, &root_);
    }

    Value drop(const Key& k)
    {
      Node_* node = findNode(k);
      if (!node) {
        throw std::out_of_range("BSTree: ключ не найден");
      }
      Value result = node->data_.second;
      eraseNode(node);
      --size_;
      return result;
    }

    const_iterator rotateRight(const_iterator pos)
    {
      Node_* node = pos.node_;
      if (!node || !node->parent_) {
        throw std::logic_error("BSTree: cannot rotate");
      }
      Node_* parent = node->parent_;
      if (parent->left_ != node) {
        throw std::logic_error("BSTree: rotateRight requires left child");
      }
      Node_* grandparent = parent->parent_;
      parent->left_ = node->right_;
      if (node->right_) {
        node->right_->parent_ = parent;
      }
      node->right_ = parent;
      parent->parent_ = node;
      node->parent_ = grandparent;
      if (!grandparent) {
        root_ = node;
      } else if (grandparent->left_ == parent) {
        grandparent->left_ = node;
      } else {
        grandparent->right_ = node;
      }
      Node_** addr = const_cast< Node_** >(&root_);
      return const_iterator(node, addr);
    }

    const_iterator rotateLeft(const_iterator pos)
    {
      Node_* node = pos.node_;
      if (!node || !node->parent_) {
        throw std::logic_error("BSTree: cannot rotate");
      }
      Node_* parent = node->parent_;
      if (parent->right_ != node) {
        throw std::logic_error("BSTree: rotateLeft requires right child");
      }
      Node_* grandparent = parent->parent_;
      parent->right_ = node->left_;
      if (node->left_) {
        node->left_->parent_ = parent;
      }
      node->left_ = parent;
      parent->parent_ = node;
      node->parent_ = grandparent;
      if (!grandparent) {
        root_ = node;
      } else if (grandparent->left_ == parent) {
        grandparent->left_ = node;
      } else {
        grandparent->right_ = node;
      }
      Node_** addr = const_cast< Node_** >(&root_);
      return const_iterator(node, addr);
    }

    const_iterator rotateLargeRight(const_iterator pos)
    {
      Node_* node = pos.node_;
      if (!node || !node->parent_ || !node->parent_->parent_) {
        throw std::logic_error("BSTree: cannot large rotate");
      }
      Node_** addr = const_cast< Node_** >(&root_);
      rotateLeft(const_iterator(node, addr));
      return rotateRight(const_iterator(node, addr));
    }

    const_iterator rotateLargeLeft(const_iterator pos)
    {
      Node_* node = pos.node_;
      if (!node || !node->parent_ || !node->parent_->parent_) {
        throw std::logic_error("BSTree: cannot large rotate");
      }
      Node_** addr = const_cast< Node_** >(&root_);
      rotateRight(const_iterator(node, addr));
      return rotateLeft(const_iterator(node, addr));
    }

    size_t height(const_iterator pos) const
    {
      return subtreeHeight(pos.node_);
    }

    size_t height() const
    {
      return subtreeHeight(root_);
    }

    BSTree(const BSTree& other):
      root_(nullptr),
      size_(0),
      cmp_(other.cmp_)
    {
      root_ = cloneSubtree(other.root_, nullptr);
      size_ = other.size_;
    }

    BSTree& operator=(const BSTree& other)
    {
      if (this != &other) {
        BSTree tmp(other);
        swap(tmp);
      }
      return *this;
    }

    BSTree(BSTree&& other):
      root_(other.root_),
      size_(other.size_),
      cmp_(other.cmp_)
    {
      other.root_ = nullptr;
      other.size_ = 0;
    }

    BSTree& operator=(BSTree&& other)
    {
      if (this != &other) {
        clear();
        root_ = other.root_;
        size_ = other.size_;
        cmp_ = other.cmp_;
        other.root_ = nullptr;
        other.size_ = 0;
      }
      return *this;
    }

    void swap(BSTree& other)
    {
      Node_* tmpRoot = root_;
      root_ = other.root_;
      other.root_ = tmpRoot;

      size_t tmpSize = size_;
      size_ = other.size_;
      other.size_ = tmpSize;

      Compare tmpCmp = cmp_;
      cmp_ = other.cmp_;
      other.cmp_ = tmpCmp;
    }

  private:
    using Node_ = detail::BSTNode< Key, Value >;

    Node_* root_;
    size_t size_;
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

    void transplant(Node_* target, Node_* replacement)
    {
      if (!target->parent_) {
        root_ = replacement;
      } else if (target == target->parent_->left_) {
        target->parent_->left_ = replacement;
      } else {
        target->parent_->right_ = replacement;
      }
      if (replacement) {
        replacement->parent_ = target->parent_;
      }
    }

    void eraseNode(Node_* node)
    {
      if (!node->left_) {
        transplant(node, node->right_);
      } else if (!node->right_) {
        transplant(node, node->left_);
      } else {
        Node_* successor = detail::leftmost< Key, Value >(node->right_);
        if (successor->parent_ != node) {
          transplant(successor, successor->right_);
          successor->right_ = node->right_;
          successor->right_->parent_ = successor;
        }
        transplant(node, successor);
        successor->left_ = node->left_;
        successor->left_->parent_ = successor;
      }
      delete node;
    }

    static size_t subtreeHeight(const Node_* node)
    {
      if (!node) {
        return 0;
      }
      const size_t lh = subtreeHeight(node->left_);
      const size_t rh = subtreeHeight(node->right_);
      return 1 + (lh > rh ? lh : rh);
    }

    static Node_* cloneSubtree(const Node_* src, Node_* parent)
    {
      if (!src) {
        return nullptr;
      }
      Node_* copy = new Node_(src->data_.first, src->data_.second, parent);
      try {
        copy->left_ = cloneSubtree(src->left_, copy);
        copy->right_ = cloneSubtree(src->right_, copy);
      } catch (...) {
        freeSubtree(copy);
        throw;
      }
      return copy;
    }

    template< class K, class V >
    friend class BSTIterator;
    template< class K, class V >
    friend class BSTConstIterator;
  };

  template< class Key, class Value, class Compare >
  void swap(BSTree< Key, Value, Compare >& a, BSTree< Key, Value, Compare >& b)
  {
    a.swap(b);
  }

}

#endif
