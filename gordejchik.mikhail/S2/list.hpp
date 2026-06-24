#ifndef GORDEJCHIK_LIST_HPP
#define GORDEJCHIK_LIST_HPP

#include <iostream>
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

    iterator insert(const_iterator pos, const T& value);

    iterator erase(iterator pos);
    void clear() noexcept;
    void swap(List& other) noexcept;

  private:
    using BaseNode = detail::BaseNode;
    using Node = detail::Node< T >;

    void insertBefore(BaseNode* pos, Node* node) noexcept;

    BaseNode fake_;
    size_t size_;
  };

  template< class T >
  List< T >::List() noexcept:
    fake_(),
    size_(0)
  {}

  template< class T >
  List< T >::List(const List& other):
    fake_(),
    size_(0)
  {
    try {
      for (auto it = other.cbegin(); it != other.cend(); ++it) {
        pushBack(*it);
      }
    } catch (...) {
      clear();
      throw;
    }
  }

  template< class T >
  List< T >::List(List&& other) noexcept:
    fake_(),
    size_(0)
  {
    swap(other);
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
  void List< T >::insertBefore(BaseNode* pos, Node* node) noexcept
  {
    node->next_ = pos;
    node->prev_ = pos->prev_;
    pos->prev_->next_ = node;
    pos->prev_ = node;
    ++size_;
  }

  template< class T >
  typename List< T >::iterator List< T >::begin() noexcept
  {
    return iterator(fake_.next_);
  }

  template< class T >
  typename List< T >::iterator List< T >::end() noexcept
  {
    return iterator(&fake_);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::begin() const noexcept
  {
    return const_iterator(fake_.next_);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::end() const noexcept
  {
    return const_iterator(&fake_);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::cbegin() const noexcept
  {
    return const_iterator(fake_.next_);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::cend() const noexcept
  {
    return const_iterator(&fake_);
  }

  template< class T >
  T& List< T >::front()
  {
    return static_cast< Node* >(fake_.next_)->value_;
  }

  template< class T >
  const T& List< T >::front() const
  {
    return static_cast< const Node* >(fake_.next_)->value_;
  }

  template< class T >
  T& List< T >::back()
  {
    return static_cast< Node* >(fake_.prev_)->value_;
  }

  template< class T >
  const T& List< T >::back() const
  {
    return static_cast< const Node* >(fake_.prev_)->value_;
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
    insertBefore(fake_.next_, new Node(value));
  }

  template< class T >
  void List< T >::pushFront(T&& value)
  {
    insertBefore(fake_.next_, new Node(static_cast< T&& >(value)));
  }

  template< class T >
  void List< T >::pushBack(const T& value)
  {
    insertBefore(&fake_, new Node(value));
  }

  template< class T >
  void List< T >::pushBack(T&& value)
  {
    insertBefore(&fake_, new Node(static_cast< T&& >(value)));
  }

  template< class T >
  void List< T >::popFront()
  {
    erase(begin());
  }

  template< class T >
  void List< T >::popBack()
  {
    erase(iterator(fake_.prev_));
  }

  template< class T >
  typename List< T >::iterator List< T >::insert(const_iterator pos, const T& value)
  {
    Node* node = new Node(value);
    insertBefore(const_cast< BaseNode* >(pos.node_), node);
    return iterator(node);
  }

  template< class T >
  typename List< T >::iterator List< T >::erase(iterator pos)
  {
    BaseNode* node = pos.node_;
    BaseNode* next = node->next_;
    node->prev_->next_ = next;
    next->prev_ = node->prev_;
    delete static_cast< Node* >(node);
    --size_;
    return iterator(next);
  }

  template< class T >
  void List< T >::clear() noexcept
  {
    BaseNode* cur = fake_.next_;
    while (cur != &fake_) {
      BaseNode* next = cur->next_;
      delete static_cast< Node* >(cur);
      cur = next;
    }
    fake_.next_ = &fake_;
    fake_.prev_ = &fake_;
    size_ = 0;
  }

  template< class T >
  void List< T >::swap(List& other) noexcept
  {
    const bool thisEmpty = empty();
    const bool otherEmpty = other.empty();

    BaseNode* tmpNext = fake_.next_;
    BaseNode* tmpPrev = fake_.prev_;
    size_t tmpSize = size_;

    fake_.next_ = other.fake_.next_;
    fake_.prev_ = other.fake_.prev_;
    size_ = other.size_;

    other.fake_.next_ = tmpNext;
    other.fake_.prev_ = tmpPrev;
    other.size_ = tmpSize;

    if (otherEmpty) {
      fake_.next_ = &fake_;
      fake_.prev_ = &fake_;
    } else {
      fake_.next_->prev_ = &fake_;
      fake_.prev_->next_ = &fake_;
    }

    if (thisEmpty) {
      other.fake_.next_ = &other.fake_;
      other.fake_.prev_ = &other.fake_;
    } else {
      other.fake_.next_->prev_ = &other.fake_;
      other.fake_.prev_->next_ = &other.fake_;
    }
  }
}

#endif
