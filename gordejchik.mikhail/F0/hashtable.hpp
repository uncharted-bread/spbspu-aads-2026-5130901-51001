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
    Value& at(const Key& key);
    const Value& at(const Key& key) const;
    bool contains(const Key& key) const;
    void erase(const Key& key);

    size_t size() const;
    bool empty() const;
    size_t capacity() const;

  private:
    static const size_t DEFAULT_CAPACITY = 16;
    static const size_t GROW_FACTOR = 2;

    struct Slot {
      Key key_;
      Value value_;
      size_t psl_;
      bool occupied_;
    };

    Slot* slots_;
    size_t capacity_;
    size_t size_;
    Hash hash_;
    Equal equal_;

    size_t findIndex(const Key& key) const;
    bool shouldGrow() const;
    void rehash(size_t newCapacity);
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
  size_t HashTable< Key, Value, Hash, Equal >::findIndex(
      const Key& key) const
  {
    size_t idx = hash_(key) % capacity_;
    size_t psl = 0;
    while (slots_[idx].occupied_) {
      if (psl > slots_[idx].psl_) {
        return capacity_;
      }
      if (equal_(slots_[idx].key_, key)) {
        return idx;
      }
      ++psl;
      idx = (idx + 1) % capacity_;
    }
    return capacity_;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value& HashTable< Key, Value, Hash, Equal >::at(
      const Key& key)
  {
    size_t idx = findIndex(key);
    if (idx == capacity_) {
      throw std::out_of_range("Key not found");
    }
    return slots_[idx].value_;
  }

  template< class Key, class Value, class Hash, class Equal >
  const Value& HashTable< Key, Value, Hash, Equal >::at(
      const Key& key) const
  {
    size_t idx = findIndex(key);
    if (idx == capacity_) {
      throw std::out_of_range("Key not found");
    }
    return slots_[idx].value_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::contains(
      const Key& key) const
  {
    return findIndex(key) != capacity_;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::erase(
      const Key& key)
  {
    size_t idx = findIndex(key);
    if (idx == capacity_) {
      throw std::out_of_range("Key not found");
    }
    size_t i = idx;
    while (true) {
      size_t j = (i + 1) % capacity_;
      if (!slots_[j].occupied_ || slots_[j].psl_ == 0) {
        break;
      }
      slots_[i] = std::move(slots_[j]);
      slots_[i].psl_--;
      i = j;
    }
    slots_[i].occupied_ = false;
    slots_[i].psl_ = 0;
    --size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool
  HashTable< Key, Value, Hash, Equal >::shouldGrow() const
  {
    return size_ * 4 >= capacity_ * 3;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::rehash(
      size_t newCapacity)
  {
    Slot* newSlots = new Slot[newCapacity]();
    Slot* oldSlots = slots_;
    size_t oldCapacity = capacity_;
    slots_ = newSlots;
    capacity_ = newCapacity;
    size_ = 0;
    for (size_t i = 0; i < oldCapacity; ++i) {
      if (oldSlots[i].occupied_) {
        insertInto(slots_, capacity_,
            std::move(oldSlots[i].key_),
            std::move(oldSlots[i].value_));
      }
    }
    delete[] oldSlots;
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::insert(
      const Key& key, const Value& value)
  {
    if (shouldGrow()) {
      rehash(capacity_ * GROW_FACTOR);
    }
    insertInto(slots_, capacity_, key, value);
  }

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::insertInto(
      Slot* target, size_t cap, Key key, Value value)
  {
    size_t idx = hash_(key) % cap;
    size_t psl = 0;
    while (true) {
      if (!target[idx].occupied_) {
        target[idx].key_ = std::move(key);
        target[idx].value_ = std::move(value);
        target[idx].psl_ = psl;
        target[idx].occupied_ = true;
        ++size_;
        return;
      }
      if (equal_(target[idx].key_, key)) {
        target[idx].value_ = std::move(value);
        return;
      }
      if (psl > target[idx].psl_) {
        std::swap(key, target[idx].key_);
        std::swap(value, target[idx].value_);
        std::swap(psl, target[idx].psl_);
      }
      ++psl;
      idx = (idx + 1) % cap;
    }
  }
}

#endif
