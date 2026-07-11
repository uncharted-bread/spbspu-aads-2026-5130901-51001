#ifndef GORDEJCHIK_NODE_HPP
#define GORDEJCHIK_NODE_HPP

#include <memory>
#include <new>
#include <utility>

namespace gordejchik {
  namespace detail {
    struct BaseNode {
      BaseNode* next;
      BaseNode* prev;
    };

    template< class T >
    struct Node: public BaseNode {
      T value;
    };

    template< class T, class U >
    Node< T >* createNode(U&& value)
    {
      void* raw = ::operator new(sizeof(Node< T >));
      Node< T >* node = static_cast< Node< T >* >(raw);
      try {
        ::new (std::addressof(node->value)) T(std::forward< U >(value));
      } catch (...) {
        ::operator delete(raw);
        throw;
      }
      node->next = nullptr;
      node->prev = nullptr;
      return node;
    }

    template< class T >
    void destroyNode(Node< T >* node) noexcept
    {
      node->value.~T();
      ::operator delete(node);
    }
  }
}

#endif
