#ifndef GORDEJCHIK_QUEUE_HPP
#define GORDEJCHIK_QUEUE_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include "list.hpp"

namespace gordejchik {
  template< class T >
  class Queue {
  public:
    void push(const T& value);

    template< class... Args >
    void emplace(Args&&... args);

    const T& front() const;
    const T& back() const;
    void pop();
    bool empty() const;
    size_t size() const;

  private:
    List< T > data_;
  };

  template< class T >
  void Queue< T >::push(const T& value)
  {
    data_.pushBack(value);
  }

  template< class T >
  template< class... Args >
  void Queue< T >::emplace(Args&&... args)
  {
    data_.emplaceBack(std::forward< Args >(args)...);
  }

  template< class T >
  const T& Queue< T >::front() const
  {
    if (data_.empty()) {
      throw std::logic_error("Queue is empty");
    }
    return data_.front();
  }

  template< class T >
  const T& Queue< T >::back() const
  {
    if (data_.empty()) {
      throw std::logic_error("Queue is empty");
    }
    return data_.back();
  }

  template< class T >
  void Queue< T >::pop()
  {
    if (data_.empty()) {
      throw std::logic_error("Queue is empty");
    }
    data_.popFront();
  }

  template< class T >
  bool Queue< T >::empty() const
  {
    return data_.empty();
  }

  template< class T >
  size_t Queue< T >::size() const
  {
    return data_.size();
  }
}

#endif
