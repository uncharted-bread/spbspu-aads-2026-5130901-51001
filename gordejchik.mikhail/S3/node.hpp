#ifndef GORDEJCHIK_NODE_HPP
#define GORDEJCHIK_NODE_HPP

#include <new>
#include <type_traits>
#include <utility>

namespace gordejchik {
  namespace detail {
    struct base_node_t {
      base_node_t* next;
      base_node_t* prev;
    };

    template< class T >
    struct node_t: base_node_t {
      typename std::aligned_storage< sizeof(T), alignof(T) >::type storage;
    };

    template< class T >
    T* nodeValue(node_t< T >* node)
    {
      return static_cast< T* >(static_cast< void* >(&node->storage));
    }

    template< class T >
    const T* nodeValue(const node_t< T >* node)
    {
      return static_cast< const T* >(static_cast< const void* >(&node->storage));
    }

    template< class T >
    node_t< T >* createNode(const T& value)
    {
      node_t< T >* node = new node_t< T >;
      try {
        ::new (static_cast< void* >(&node->storage)) T(value);
      } catch (...) {
        delete node;
        throw;
      }
      node->next = nullptr;
      node->prev = nullptr;
      return node;
    }

    template< class T >
    node_t< T >* createNode(T&& value)
    {
      node_t< T >* node = new node_t< T >;
      try {
        ::new (static_cast< void* >(&node->storage)) T(std::move(value));
      } catch (...) {
        delete node;
        throw;
      }
      node->next = nullptr;
      node->prev = nullptr;
      return node;
    }

    template< class T >
    void destroyNode(node_t< T >* node)
    {
      nodeValue(node)->~T();
      delete node;
    }
  }
}

#endif
