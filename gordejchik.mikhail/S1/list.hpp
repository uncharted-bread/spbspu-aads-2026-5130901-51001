#ifndef GORDEJCHIK_LIST_HPP
#define GORDEJCHIK_LIST_HPP

#include <iostream>
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
    fake_{&fake_, &fake_},
    size_(0)
  {}

  template< class T >
  List< T >::List(const List& other):
    fake_{&fake_, &fake_},
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
    fake_{&fake_, &fake_},
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
    node->next = pos;
    node->prev = pos->prev;
    pos->prev->next = node;
    pos->prev = node;
    ++size_;
  }

  template< class T >
  typename List< T >::iterator List< T >::begin() noexcept
  {
    return iterator(fake_.next);
  }

  template< class T >
  typename List< T >::iterator List< T >::end() noexcept
  {
    return iterator(&fake_);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::begin() const noexcept
  {
    return const_iterator(fake_.next);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::end() const noexcept
  {
    return const_iterator(&fake_);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::cbegin() const noexcept
  {
    return const_iterator(fake_.next);
  }

  template< class T >
  typename List< T >::const_iterator List< T >::cend() const noexcept
  {
    return const_iterator(&fake_);
  }

  template< class T >
  T& List< T >::front()
  {
    return static_cast< Node* >(fake_.next)->value;
  }

  template< class T >
  const T& List< T >::front() const
  {
    return static_cast< const Node* >(fake_.next)->value;
  }

  template< class T >
  T& List< T >::back()
  {
    return static_cast< Node* >(fake_.prev)->value;
  }

  template< class T >
  const T& List< T >::back() const
  {
    return static_cast< const Node* >(fake_.prev)->value;
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
    insertBefore(fake_.next, detail::createNode< T >(value));
  }

  template< class T >
  void List< T >::pushFront(T&& value)
  {
    insertBefore(fake_.next, detail::createNode< T >(std::move(value)));
  }

  template< class T >
  void List< T >::pushBack(const T& value)
  {
    insertBefore(&fake_, detail::createNode< T >(value));
  }

  template< class T >
  void List< T >::pushBack(T&& value)
  {
    insertBefore(&fake_, detail::createNode< T >(std::move(value)));
  }

  template< class T >
  void List< T >::popFront()
  {
    erase(begin());
  }

  template< class T >
  void List< T >::popBack()
  {
    erase(iterator(fake_.prev));
  }

  template< class T >
  typename List< T >::iterator List< T >::insert(const_iterator pos, const T& value)
  {
    Node* node = detail::createNode< T >(value);
    insertBefore(const_cast< BaseNode* >(pos.node_), node);
    return iterator(node);
  }

  template< class T >
  typename List< T >::iterator List< T >::erase(iterator pos)
  {
    BaseNode* node = pos.node_;
    BaseNode* next = node->next;
    node->prev->next = next;
    next->prev = node->prev;
    detail::destroyNode(static_cast< Node* >(node));
    --size_;
    return iterator(next);
  }

  template< class T >
  void List< T >::clear() noexcept
  {
    BaseNode* cur = fake_.next;
    while (cur != &fake_) {
      BaseNode* next = cur->next;
      detail::destroyNode(static_cast< Node* >(cur));
      cur = next;
    }
    fake_.next = &fake_;
    fake_.prev = &fake_;
    size_ = 0;
  }

  template< class T >
  void List< T >::swap(List& other) noexcept
  {
    const bool thisEmpty = empty();
    const bool otherEmpty = other.empty();

    BaseNode* tmpNext = fake_.next;
    BaseNode* tmpPrev = fake_.prev;
    size_t tmpSize = size_;

    fake_.next = other.fake_.next;
    fake_.prev = other.fake_.prev;
    size_ = other.size_;

    other.fake_.next = tmpNext;
    other.fake_.prev = tmpPrev;
    other.size_ = tmpSize;

    if (otherEmpty) {
      fake_.next = &fake_;
      fake_.prev = &fake_;
    } else {
      fake_.next->prev = &fake_;
      fake_.prev->next = &fake_;
    }

    if (thisEmpty) {
      other.fake_.next = &other.fake_;
      other.fake_.prev = &other.fake_;
    } else {
      other.fake_.next->prev = &other.fake_;
      other.fake_.prev->next = &other.fake_;
    }
  }
}

#endif
