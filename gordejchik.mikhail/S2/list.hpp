#ifndef GORDEJCHIK_LIST_HPP
#define GORDEJCHIK_LIST_HPP

#include <cstddef>
#include <utility>
#include "iterator.hpp"

namespace gordejchik {
  template< class T >
  class List {
  public:
    using iterator = LIter< T >;
    using const_iterator = LCIter< T >;

    List() noexcept;
    List(const List& other);
    List(List&& other) noexcept;
    ~List();

    List& operator=(const List& other);
    List& operator=(List&& other) noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    T& front();
    const T& front() const;
    T& back();
    const T& back() const;

    bool empty() const noexcept;
    size_t size() const noexcept;

    void pushFront(const T& value);
    void pushFront(T&& value);
    void pushBack(const T& value);
    void pushBack(T&& value);
    void popFront();
    void popBack();

    template< class... Args >
    iterator emplaceAfter(const_iterator pos, Args&&... args);

    template< class... Args >
    void emplaceFront(Args&&... args);

    template< class... Args >
    void emplaceBack(Args&&... args);

    iterator insert(const_iterator pos, const T& value);
    iterator erase(iterator pos);

    void clear() noexcept;
    void swap(List& other) noexcept;

  private:
    using Node = detail::node_t< T >;

    void linkBefore(Node* pos, Node* node) noexcept;
    void unlink(Node* node) noexcept;

    Node* head_;
    Node* tail_;
    size_t size_;
  };

  template< class T >
  List< T >::List() noexcept:
    head_(nullptr),
    tail_(nullptr),
    size_(0)
  {}

  template< class T >
  List< T >::List(const List& other):
    head_(nullptr),
    tail_(nullptr),
    size_(0)
  {
    try {
      for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
        pushBack(*it);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  template< class T >
  List< T >::List(List&& other) noexcept:
    head_(other.head_),
    tail_(other.tail_),
    size_(other.size_)
  {
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
  }

  template< class T >
  List< T >::~List()
  {
    clear();
  }

  template< class T >
  List< T >& List< T >::operator=(const List& other)
  {
    if (this != &other) {
      List tmp(other);
      swap(tmp);
    }
    return *this;
  }

  template< class T >
  List< T >& List< T >::operator=(List&& other) noexcept
  {
    if (this != &other) {
      clear();
      swap(other);
    }
    return *this;
  }

  template< class T >
  void List< T >::linkBefore(Node* pos, Node* node) noexcept
  {
    node->next = pos;
    node->prev = (pos != nullptr) ? pos->prev : tail_;
    if (node->prev != nullptr) {
      node->prev->next = node;
    } else {
      head_ = node;
    }
    if (pos != nullptr) {
      pos->prev = node;
    } else {
      tail_ = node;
    }
    ++size_;
  }

  template< class T >
  void List< T >::unlink(Node* node) noexcept
  {
    if (node->prev != nullptr) {
      node->prev->next = node->next;
    } else {
      head_ = node->next;
    }
    if (node->next != nullptr) {
      node->next->prev = node->prev;
    } else {
      tail_ = node->prev;
    }
    --size_;
  }

  template< class T >
  typename List< T >::iterator List< T >::begin() noexcept
  {
    return iterator(head_);
  }

  template< class T >
  typename List< T >::iterator List< T >::end() noexcept
  {
    return iterator(nullptr);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::begin() const noexcept
  {
    return const_iterator(head_);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::end() const noexcept
  {
    return const_iterator(nullptr);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::cbegin() const noexcept
  {
    return const_iterator(head_);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::cend() const noexcept
  {
    return const_iterator(nullptr);
  }

  template< class T >
  T& List< T >::front()
  {
    return head_->data;
  }

  template< class T >
  const T& List< T >::front() const
  {
    return head_->data;
  }

  template< class T >
  T& List< T >::back()
  {
    return tail_->data;
  }

  template< class T >
  const T& List< T >::back() const
  {
    return tail_->data;
  }

  template< class T >
  bool List< T >::empty() const noexcept
  {
    return size_ == 0;
  }

  template< class T >
  size_t List< T >::size() const noexcept
  {
    return size_;
  }

  template< class T >
  void List< T >::pushFront(const T& value)
  {
    linkBefore(head_, detail::createNode< T >(value));
  }

  template< class T >
  void List< T >::pushFront(T&& value)
  {
    linkBefore(head_, detail::createNode< T >(std::move(value)));
  }

  template< class T >
  void List< T >::pushBack(const T& value)
  {
    linkBefore(nullptr, detail::createNode< T >(value));
  }

  template< class T >
  void List< T >::pushBack(T&& value)
  {
    linkBefore(nullptr, detail::createNode< T >(std::move(value)));
  }

  template< class T >
  void List< T >::popFront()
  {
    erase(begin());
  }

  template< class T >
  void List< T >::popBack()
  {
    erase(iterator(tail_));
  }

  template< class T >
  template< class... Args >
  typename List< T >::iterator List< T >::emplaceAfter(const_iterator pos, Args&&... args)
  {
    Node* node = detail::createNode< T >(std::forward< Args >(args)...);
    Node* posNode = const_cast< Node* >(pos.node_);
    linkBefore(posNode->next, node);
    return iterator(node);
  }

  template< class T >
  template< class... Args >
  void List< T >::emplaceFront(Args&&... args)
  {
    linkBefore(head_, detail::createNode< T >(std::forward< Args >(args)...));
  }

  template< class T >
  template< class... Args >
  void List< T >::emplaceBack(Args&&... args)
  {
    linkBefore(nullptr, detail::createNode< T >(std::forward< Args >(args)...));
  }

  template< class T >
  typename List< T >::iterator List< T >::insert(const_iterator pos, const T& value)
  {
    Node* node = detail::createNode< T >(value);
    linkBefore(const_cast< Node* >(pos.node_), node);
    return iterator(node);
  }

  template< class T >
  typename List< T >::iterator List< T >::erase(iterator pos)
  {
    Node* node = pos.node_;
    iterator next(node->next);
    unlink(node);
    detail::destroyNode(node);
    return next;
  }

  template< class T >
  void List< T >::clear() noexcept
  {
    Node* cur = head_;
    while (cur != nullptr) {
      Node* next = cur->next;
      detail::destroyNode(cur);
      cur = next;
    }
    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
  }

  template< class T >
  void List< T >::swap(List& other) noexcept
  {
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(size_, other.size_);
  }
}

#endif
