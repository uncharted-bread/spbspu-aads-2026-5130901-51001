#ifndef GORDEJCHIK_NODE_HPP
#define GORDEJCHIK_NODE_HPP

namespace gordejchik {
  namespace detail {
    struct BaseNode {
      BaseNode* next_;
      BaseNode* prev_;

      BaseNode():
        next_(this),
        prev_(this)
      {}
    };

    template< class T >
    struct Node: public BaseNode {
      T value_;

      explicit Node(const T& v):
        value_(v)
      {}

      explicit Node(T&& v):
        value_(static_cast< T&& >(v))
      {}
    };
  }
}

#endif
