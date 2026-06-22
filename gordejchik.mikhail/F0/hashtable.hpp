#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <functional>
#include <stdexcept>
#include <cstddef>

namespace gordejchik {

  template< class Key, class Value,
      class Hash = std::hash< Key >,
      class Equal = std::equal_to< Key > >
  class HashTable {
  public:
    HashTable();
    explicit HashTable(size_t capacity);
    ~HashTable();

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    void insert(const Key& key, const Value& value);

    size_t size() const;
    bool empty() const;
    size_t capacity() const;

  private:
    static const size_t DEFAULT_CAPACITY = 16;

    struct Slot {
      Key key_;
      Value value_;
      bool occupied_;
    };

    Slot* slots_;
    size_t capacity_;
    size_t size_;
    Hash hash_;
    Equal equal_;

    void insertInto(Slot* target, size_t cap, Key key, Value value);
  };

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable():
    slots_(new Slot[DEFAULT_CAPACITY]()),
    capacity_(DEFAULT_CAPACITY),
    size_(0),
    hash_(),
    equal_()
  {}

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(
      size_t capacity):
    slots_(nullptr),
    capacity_(capacity < 1 ? DEFAULT_CAPACITY : capacity),
    size_(0),
    hash_(),
    equal_()
  {
    slots_ = new Slot[capacity_]();
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::~HashTable()
  {
    delete[] slots_;
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t HashTable< Key, Value, Hash, Equal >::size() const
  {
    return size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::empty() const
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t
  HashTable< Key, Value, Hash, Equal >::capacity() const
  {
    return capacity_;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::insert(
      const Key& key, const Value& value)
  {
    insertInto(slots_, capacity_, key, value);
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::insertInto(
      Slot* target, size_t cap, Key key, Value value)
  {
    size_t idx = hash_(key) % cap;
    while (true) {
      if (!target[idx].occupied_) {
        target[idx].key_ = std::move(key);
        target[idx].value_ = std::move(value);
        target[idx].occupied_ = true;
        ++size_;
        return;
      }
      if (equal_(target[idx].key_, key)) {
        target[idx].value_ = std::move(value);
        return;
      }
      idx = (idx + 1) % cap;
    }
  }
}

#endif
