#ifndef GORDEJCHIK_NODE_HPP
#define GORDEJCHIK_NODE_HPP

#include <memory>
#include <new>
#include <utility>

namespace gordejchik {
  namespace detail {
    template< class T >
    struct node_t {
      T data;
      node_t< T >* next;
      node_t< T >* prev;
    };

    template< class T, class... Args >
    node_t< T >* createNode(Args&&... args)
    {
      void* raw = ::operator new(sizeof(node_t< T >));
      node_t< T >* node = static_cast< node_t< T >* >(raw);
      try {
        ::new (std::addressof(node->data)) T(std::forward< Args >(args)...);
      } catch (...) {
        ::operator delete(raw);
        throw;
      }
      node->next = nullptr;
      node->prev = nullptr;
      return node;
    }

    template< class T >
    void destroyNode(node_t< T >* node) noexcept
    {
      node->data.~T();
      ::operator delete(node);
    }
  }
}

#endif
