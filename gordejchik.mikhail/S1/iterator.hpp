#ifndef GORDEJCHIK_ITERATOR_HPP
#define GORDEJCHIK_ITERATOR_HPP

#include "node.hpp"

namespace gordejchik {
  template< class T >
  class List;

  template< class T >
  class LCIter;

  template< class T >
  class LIter {
    friend class List< T >;
    friend class LCIter< T >;
  public:
    LIter() noexcept;
    T& operator*() const noexcept;
    T* operator->() const noexcept;
    LIter& operator++() noexcept;
    LIter operator++(int) noexcept;
    LIter& operator--() noexcept;
    LIter operator--(int) noexcept;
    bool operator==(const LIter& rhs) const noexcept;
    bool operator!=(const LIter& rhs) const noexcept;
  private:
    using BaseNode = detail::BaseNode;
    using Node = detail::Node< T >;
    explicit LIter(BaseNode* node) noexcept;
    BaseNode* node_;
  };

  template< class T >
  class LCIter {
    friend class List< T >;
  public:
    LCIter() noexcept;
    LCIter(const LIter< T >& other) noexcept;
    const T& operator*() const noexcept;
    const T* operator->() const noexcept;
    LCIter& operator++() noexcept;
    LCIter operator++(int) noexcept;
    LCIter& operator--() noexcept;
    LCIter operator--(int) noexcept;
    bool operator==(const LCIter& rhs) const noexcept;
    bool operator!=(const LCIter& rhs) const noexcept;
  private:
    using BaseNode = detail::BaseNode;
    using Node = detail::Node< T >;
    explicit LCIter(const BaseNode* node) noexcept;
    const BaseNode* node_;
  };

  template< class T >
  LIter< T >::LIter() noexcept:
    node_(nullptr)
  {}

  template< class T >
  LIter< T >::LIter(BaseNode* node) noexcept:
    node_(node)
  {}

  template< class T >
  T& LIter< T >::operator*() const noexcept
  {
    return static_cast< Node* >(node_)->value;
  }

  template< class T >
  T* LIter< T >::operator->() const noexcept
  {
    return &(static_cast< Node* >(node_)->value);
  }

  template< class T >
  LIter< T >& LIter< T >::operator++() noexcept
  {
    node_ = node_->next;
    return *this;
  }

  template< class T >
  LIter< T > LIter< T >::operator++(int) noexcept
  {
    LIter tmp(*this);
    node_ = node_->next;
    return tmp;
  }

  template< class T >
  LIter< T >& LIter< T >::operator--() noexcept
  {
    node_ = node_->prev;
    return *this;
  }

  template< class T >
  LIter< T > LIter< T >::operator--(int) noexcept
  {
    LIter tmp(*this);
    node_ = node_->prev;
    return tmp;
  }

  template< class T >
  bool LIter< T >::operator==(const LIter& rhs) const noexcept
  {
    return node_ == rhs.node_;
  }

  template< class T >
  bool LIter< T >::operator!=(const LIter& rhs) const noexcept
  {
    return node_ != rhs.node_;
  }

  template< class T >
  LCIter< T >::LCIter() noexcept:
    node_(nullptr)
  {}

  template< class T >
  LCIter< T >::LCIter(const LIter< T >& other) noexcept:
    node_(other.node_)
  {}

  template< class T >
  LCIter< T >::LCIter(const BaseNode* node) noexcept:
    node_(node)
  {}

  template< class T >
  const T& LCIter< T >::operator*() const noexcept
  {
    return static_cast< const Node* >(node_)->value;
  }

  template< class T >
  const T* LCIter< T >::operator->() const noexcept
  {
    return &(static_cast< const Node* >(node_)->value);
  }

  template< class T >
  LCIter< T >& LCIter< T >::operator++() noexcept
  {
    node_ = node_->next;
    return *this;
  }

  template< class T >
  LCIter< T > LCIter< T >::operator++(int) noexcept
  {
    LCIter tmp(*this);
    node_ = node_->next;
    return tmp;
  }

  template< class T >
  LCIter< T >& LCIter< T >::operator--() noexcept
  {
    node_ = node_->prev;
    return *this;
  }

  template< class T >
  LCIter< T > LCIter< T >::operator--(int) noexcept
  {
    LCIter tmp(*this);
    node_ = node_->prev;
    return tmp;
  }

  template< class T >
  bool LCIter< T >::operator==(const LCIter& rhs) const noexcept
  {
    return node_ == rhs.node_;
  }

  template< class T >
  bool LCIter< T >::operator!=(const LCIter& rhs) const noexcept
  {
    return node_ != rhs.node_;
  }
}

#endif
