#ifndef GORDEJCHIK_STACK_HPP
#define GORDEJCHIK_STACK_HPP

#include <cstddef>
#include <stdexcept>
#include "list.hpp"

namespace gordejchik {
  template< class T >
  class Stack {
  public:
    void push(const T& value);
    const T& top() const;
    void pop();
    bool empty() const;
    size_t size() const;

  private:
    List< T > data_;
  };

  template< class T >
  void Stack< T >::push(const T& value)
  {
    data_.pushBack(value);
  }

  template< class T >
  const T& Stack< T >::top() const
  {
    if (data_.empty()) {
      throw std::logic_error("Stack is empty");
    }
    return data_.back();
  }

  template< class T >
  void Stack< T >::pop()
  {
    if (data_.empty()) {
      throw std::logic_error("Stack is empty");
    }
    data_.popBack();
  }

  template< class T >
  bool Stack< T >::empty() const
  {
    return data_.empty();
  }

  template< class T >
  size_t Stack< T >::size() const
  {
    return data_.size();
  }
}

#endif
