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
      std::pair< Key, Value > data;
      BSTNode* left;
      BSTNode* right;
      BSTNode* parent;
      size_t height;

      BSTNode(const Key& k, const Value& v, BSTNode* parentNode):
        data(k, v),
        left(nullptr),
        right(nullptr),
        parent(parentNode),
        height(1)
      {}
    };

    template< class Key, class Value >
    BSTNode< Key, Value >* leftmost(BSTNode< Key, Value >* node)
    {
      if (!node) {
        return nullptr;
      }
      while (node->left) {
        node = node->left;
      }
      return node;
    }

    template< class Key, class Value >
    BSTNode< Key, Value >* rightmost(BSTNode< Key, Value >* node)
    {
      if (!node) {
        return nullptr;
      }
      while (node->right) {
        node = node->right;
      }
      return node;
    }

    template< class Key, class Value >
    BSTNode< Key, Value >* nextInOrder(BSTNode< Key, Value >* node)
    {
      if (node->right) {
        return leftmost< Key, Value >(node->right);
      }
      BSTNode< Key, Value >* parent = node->parent;
      while (parent && node == parent->right) {
        node = parent;
        parent = parent->parent;
      }
      return parent;
    }

    template< class Key, class Value >
    BSTNode< Key, Value >* prevInOrder(BSTNode< Key, Value >* node)
    {
      if (node->left) {
        return rightmost< Key, Value >(node->left);
      }
      BSTNode< Key, Value >* parent = node->parent;
      while (parent && node == parent->left) {
        node = parent;
        parent = parent->parent;
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
      return node_->data;
    }

    pointer operator->() const
    {
      return &node_->data;
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
    using Node = detail::BSTNode< Key, Value >;

    Node* node_;
    Node** rootAddr_;

    BSTIterator(Node* node, Node** rootAddr):
      node_(node),
      rootAddr_(rootAddr)
    {}
  };

  template< class Key, class Value >
  class BSTConstIterator {
    template< class K, class V, class C >
    friend class BSTree;

  public:
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
      return node_->data;
    }

    pointer operator->() const
    {
      return &node_->data;
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
    using Node = detail::BSTNode< Key, Value >;

    Node* node_;
    Node* const* rootAddr_;

    BSTConstIterator(Node* node, Node* const* rootAddr):
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

    BSTree(const BSTree& other):
      root_(nullptr),
      size_(0),
      cmp_(other.cmp_)
    {
      root_ = cloneSubtree(other.root_, nullptr);
      size_ = other.size_;
    }

    BSTree(BSTree&& other):
      root_(other.root_),
      size_(other.size_),
      cmp_(other.cmp_)
    {
      other.root_ = nullptr;
      other.size_ = 0;
    }

    ~BSTree()
    {
      clear();
    }

    BSTree& operator=(const BSTree& other)
    {
      if (this != &other) {
        BSTree tmp(other);
        swap(tmp);
      }
      return *this;
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

    Value& operator[](const Key& k)
    {
      bool inserted = false;
      Node* node = insertNode(k, Value(), inserted);
      return node->data.second;
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
      bool inserted = false;
      Node* node = insertNode(k, v, inserted);
      if (!inserted) {
        node->data.second = v;
      }
    }

    const Value& get(const Key& k) const
    {
      const Node* node = findNode(k);
      if (!node) {
        throw std::out_of_range("BSTree: key not found");
      }
      return node->data.second;
    }

    Value& get(const Key& k)
    {
      Node* node = findNode(k);
      if (!node) {
        throw std::out_of_range("BSTree: key not found");
      }
      return node->data.second;
    }

    bool contains(const Key& k) const
    {
      return findNode(k) != nullptr;
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
      return const_iterator(detail::leftmost< Key, Value >(root_), &root_);
    }

    const_iterator cend() const
    {
      return const_iterator(nullptr, &root_);
    }

    iterator find(const Key& k)
    {
      return iterator(findNode(k), &root_);
    }

    const_iterator find(const Key& k) const
    {
      return const_iterator(findNode(k), &root_);
    }

    iterator erase(const_iterator pos)
    {
      Node* node = pos.node_;
      if (!node) {
        return end();
      }
      Node* next = detail::nextInOrder< Key, Value >(node);
      eraseNode(node);
      return iterator(next, &root_);
    }

    Value drop(const Key& k)
    {
      Node* node = findNode(k);
      if (!node) {
        throw std::out_of_range("BSTree: key not found");
      }
      Value result = node->data.second;
      eraseNode(node);
      return result;
    }

    const_iterator rotateRight(const_iterator pos)
    {
      Node* node = pos.node_;
      if (!node || !node->parent) {
        throw std::logic_error("BSTree: cannot rotate");
      }
      if (node->parent->left != node) {
        throw std::logic_error("BSTree: rotateRight requires left child");
      }
      rotateRightNode(node);
      updateHeightsUp(node->parent);
      return const_iterator(node, &root_);
    }

    const_iterator rotateLeft(const_iterator pos)
    {
      Node* node = pos.node_;
      if (!node || !node->parent) {
        throw std::logic_error("BSTree: cannot rotate");
      }
      if (node->parent->right != node) {
        throw std::logic_error("BSTree: rotateLeft requires right child");
      }
      rotateLeftNode(node);
      updateHeightsUp(node->parent);
      return const_iterator(node, &root_);
    }

    const_iterator rotateLargeRight(const_iterator pos)
    {
      Node* node = pos.node_;
      if (!node || !node->parent || !node->parent->parent) {
        throw std::logic_error("BSTree: cannot do large rotation");
      }
      Node* parent = node->parent;
      if (parent->right != node || parent->parent->left != parent) {
        throw std::logic_error("BSTree: invalid position for large right rotation");
      }
      rotateLeft(const_iterator(node, &root_));
      return rotateRight(const_iterator(node, &root_));
    }

    const_iterator rotateLargeLeft(const_iterator pos)
    {
      Node* node = pos.node_;
      if (!node || !node->parent || !node->parent->parent) {
        throw std::logic_error("BSTree: cannot do large rotation");
      }
      Node* parent = node->parent;
      if (parent->left != node || parent->parent->right != parent) {
        throw std::logic_error("BSTree: invalid position for large left rotation");
      }
      rotateRight(const_iterator(node, &root_));
      return rotateLeft(const_iterator(node, &root_));
    }

    size_t height(const_iterator pos) const
    {
      return nodeHeight(pos.node_);
    }

    size_t height() const
    {
      return nodeHeight(root_);
    }

    void swap(BSTree& other)
    {
      std::swap(root_, other.root_);
      std::swap(size_, other.size_);
      std::swap(cmp_, other.cmp_);
    }

  protected:
    using Node = detail::BSTNode< Key, Value >;

    static Node* nodeOf(const_iterator pos)
    {
      return pos.node_;
    }

    iterator makeIterator(Node* node)
    {
      return iterator(node, &root_);
    }

    Node* findNode(const Key& k) const
    {
      Node* cur = root_;
      while (cur) {
        if (cmp_(k, cur->data.first)) {
          cur = cur->left;
        } else if (cmp_(cur->data.first, k)) {
          cur = cur->right;
        } else {
          return cur;
        }
      }
      return nullptr;
    }

    Node* insertNode(const Key& k, const Value& v, bool& inserted)
    {
      inserted = false;
      if (!root_) {
        root_ = new Node(k, v, nullptr);
        ++size_;
        inserted = true;
        return root_;
      }
      Node* cur = root_;
      while (true) {
        if (cmp_(k, cur->data.first)) {
          if (!cur->left) {
            cur->left = new Node(k, v, cur);
            ++size_;
            updateHeightsUp(cur);
            inserted = true;
            return cur->left;
          }
          cur = cur->left;
        } else if (cmp_(cur->data.first, k)) {
          if (!cur->right) {
            cur->right = new Node(k, v, cur);
            ++size_;
            updateHeightsUp(cur);
            inserted = true;
            return cur->right;
          }
          cur = cur->right;
        } else {
          return cur;
        }
      }
    }

    Node* eraseNode(Node* node)
    {
      Node* fixup = node->parent;
      if (!node->left) {
        transplant(node, node->right);
      } else if (!node->right) {
        transplant(node, node->left);
      } else {
        Node* successor = detail::leftmost< Key, Value >(node->right);
        fixup = successor;
        if (successor->parent != node) {
          fixup = successor->parent;
          transplant(successor, successor->right);
          successor->right = node->right;
          successor->right->parent = successor;
        }
        transplant(node, successor);
        successor->left = node->left;
        successor->left->parent = successor;
      }
      delete node;
      --size_;
      updateHeightsUp(fixup);
      return fixup;
    }

    Node* rotateRightNode(Node* node)
    {
      Node* parent = node->parent;
      Node* grandparent = parent->parent;
      parent->left = node->right;
      if (node->right) {
        node->right->parent = parent;
      }
      node->right = parent;
      parent->parent = node;
      node->parent = grandparent;
      if (!grandparent) {
        root_ = node;
      } else if (grandparent->left == parent) {
        grandparent->left = node;
      } else {
        grandparent->right = node;
      }
      updateHeight(parent);
      updateHeight(node);
      return node;
    }

    Node* rotateLeftNode(Node* node)
    {
      Node* parent = node->parent;
      Node* grandparent = parent->parent;
      parent->right = node->left;
      if (node->left) {
        node->left->parent = parent;
      }
      node->left = parent;
      parent->parent = node;
      node->parent = grandparent;
      if (!grandparent) {
        root_ = node;
      } else if (grandparent->left == parent) {
        grandparent->left = node;
      } else {
        grandparent->right = node;
      }
      updateHeight(parent);
      updateHeight(node);
      return node;
    }

    static size_t nodeHeight(const Node* node)
    {
      return node ? node->height : 0;
    }

    static void updateHeight(Node* node)
    {
      const size_t lh = nodeHeight(node->left);
      const size_t rh = nodeHeight(node->right);
      node->height = 1 + (lh > rh ? lh : rh);
    }

  private:
    Node* root_;
    size_t size_;
    Compare cmp_;

    void transplant(Node* target, Node* replacement)
    {
      if (!target->parent) {
        root_ = replacement;
      } else if (target == target->parent->left) {
        target->parent->left = replacement;
      } else {
        target->parent->right = replacement;
      }
      if (replacement) {
        replacement->parent = target->parent;
      }
    }

    static void updateHeightsUp(Node* node)
    {
      while (node) {
        updateHeight(node);
        node = node->parent;
      }
    }

    static void freeSubtree(Node* root)
    {
      Node* cur = root;
      while (cur) {
        if (cur->left) {
          cur = cur->left;
        } else if (cur->right) {
          cur = cur->right;
        } else {
          Node* parent = (cur == root) ? nullptr : cur->parent;
          if (parent) {
            if (parent->left == cur) {
              parent->left = nullptr;
            } else {
              parent->right = nullptr;
            }
          }
          delete cur;
          cur = parent;
        }
      }
    }

    static Node* cloneSubtree(const Node* src, Node* parent)
    {
      if (!src) {
        return nullptr;
      }
      Node* root = nullptr;
      try {
        root = new Node(src->data.first, src->data.second, parent);
        root->height = src->height;
        const Node* from = src;
        Node* to = root;
        while (true) {
          if (from->left && !to->left) {
            from = from->left;
            to->left = new Node(from->data.first, from->data.second, to);
            to->left->height = from->height;
            to = to->left;
          } else if (from->right && !to->right) {
            from = from->right;
            to->right = new Node(from->data.first, from->data.second, to);
            to->right->height = from->height;
            to = to->right;
          } else if (from == src) {
            break;
          } else {
            from = from->parent;
            to = to->parent;
          }
        }
      } catch (...) {
        freeSubtree(root);
        throw;
      }
      return root;
    }
  };

  template< class Key, class Value, class Compare >
  void swap(BSTree< Key, Value, Compare >& a, BSTree< Key, Value, Compare >& b)
  {
    a.swap(b);
  }
}

#endif
