#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>

namespace gordejchik {

  template< class Key, class Value,
      class Hash = std::hash< Key >,
      class Equal = std::equal_to< Key > >
  class HashTable {
  public:
    using value_type = std::pair< Key, Value >;

    class Iterator;
    class ConstIterator;

    HashTable();
    explicit HashTable(size_t capacity);
    HashTable(const HashTable& other);
    HashTable(HashTable&& other) noexcept;
    ~HashTable();

    HashTable& operator=(const HashTable& other);
    HashTable& operator=(HashTable&& other) noexcept;

    void insert(const Key& key, const Value& value);
    Value& at(const Key& key);
    const Value& at(const Key& key) const;
    Iterator find(const Key& key);
    ConstIterator find(const Key& key) const;
    bool contains(const Key& key) const;
    void erase(const Key& key);

    size_t size() const;
    bool empty() const;
    size_t capacity() const;

    Iterator begin();
    Iterator end();
    ConstIterator begin() const;
    ConstIterator end() const;
    ConstIterator cbegin() const;
    ConstIterator cend() const;

  private:
    static constexpr size_t DEFAULT_CAPACITY = 16;
    static constexpr size_t GROW_FACTOR = 2;

    struct Slot {
      value_type data;
      size_t psl;
      bool occupied;
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
  class HashTable< Key, Value, Hash, Equal >::Iterator {
    friend class HashTable;
  public:
    value_type& operator*() const;
    value_type* operator->() const;
    Iterator& operator++();
    Iterator operator++(int);
    bool operator==(const Iterator& rhs) const;
    bool operator!=(const Iterator& rhs) const;

  private:
    Slot* current_;
    Slot* end_;

    Iterator(Slot* current, Slot* end);
    void skipEmpty();
  };

  template< class Key, class Value, class Hash, class Equal >
  class HashTable< Key, Value, Hash, Equal >::ConstIterator {
    friend class HashTable;
  public:
    ConstIterator(const Iterator& it);

    const value_type& operator*() const;
    const value_type* operator->() const;
    ConstIterator& operator++();
    ConstIterator operator++(int);
    bool operator==(const ConstIterator& rhs) const;
    bool operator!=(const ConstIterator& rhs) const;

  private:
    const Slot* current_;
    const Slot* end_;

    ConstIterator(const Slot* current, const Slot* end);
    void skipEmpty();
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
  HashTable< Key, Value, Hash, Equal >::HashTable(
      const HashTable& other):
    slots_(new Slot[other.capacity_]()),
    capacity_(other.capacity_),
    size_(0),
    hash_(other.hash_),
    equal_(other.equal_)
  {
    try {
      for (size_t i = 0; i < capacity_; ++i) {
        slots_[i] = other.slots_[i];
      }
    } catch (...) {
      delete[] slots_;
      throw;
    }
    size_ = other.size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::HashTable(
      HashTable&& other) noexcept:
    slots_(other.slots_),
    capacity_(other.capacity_),
    size_(other.size_),
    hash_(std::move(other.hash_)),
    equal_(std::move(other.equal_))
  {
    other.slots_ = nullptr;
    other.capacity_ = 0;
    other.size_ = 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::~HashTable()
  {
    delete[] slots_;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >&
  HashTable< Key, Value, Hash, Equal >::operator=(
      const HashTable& other)
  {
    if (this != &other) {
      Slot* newSlots = new Slot[other.capacity_]();
      try {
        for (size_t i = 0; i < other.capacity_; ++i) {
          newSlots[i] = other.slots_[i];
        }
      } catch (...) {
        delete[] newSlots;
        throw;
      }
      delete[] slots_;
      slots_ = newSlots;
      capacity_ = other.capacity_;
      size_ = other.size_;
      hash_ = other.hash_;
      equal_ = other.equal_;
    }
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >&
  HashTable< Key, Value, Hash, Equal >::operator=(
      HashTable&& other) noexcept
  {
    if (this != &other) {
      delete[] slots_;
      slots_ = other.slots_;
      capacity_ = other.capacity_;
      size_ = other.size_;
      hash_ = std::move(other.hash_);
      equal_ = std::move(other.equal_);
      other.slots_ = nullptr;
      other.capacity_ = 0;
      other.size_ = 0;
    }
    return *this;
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
    while (slots_[idx].occupied) {
      if (psl > slots_[idx].psl) {
        return capacity_;
      }
      if (equal_(slots_[idx].data.first, key)) {
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
    return slots_[idx].data.second;
  }

  template< class Key, class Value, class Hash, class Equal >
  const Value& HashTable< Key, Value, Hash, Equal >::at(
      const Key& key) const
  {
    size_t idx = findIndex(key);
    if (idx == capacity_) {
      throw std::out_of_range("Key not found");
    }
    return slots_[idx].data.second;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::Iterator
  HashTable< Key, Value, Hash, Equal >::find(const Key& key)
  {
    size_t idx = findIndex(key);
    if (idx == capacity_) {
      return end();
    }
    return Iterator(slots_ + idx, slots_ + capacity_);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::ConstIterator
  HashTable< Key, Value, Hash, Equal >::find(const Key& key) const
  {
    size_t idx = findIndex(key);
    if (idx == capacity_) {
      return end();
    }
    return ConstIterator(slots_ + idx, slots_ + capacity_);
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
      if (!slots_[j].occupied || slots_[j].psl == 0) {
        break;
      }
      slots_[i] = std::move(slots_[j]);
      --slots_[i].psl;
      i = j;
    }
    slots_[i].occupied = false;
    slots_[i].psl = 0;
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
      if (oldSlots[i].occupied) {
        insertInto(slots_, capacity_,
            std::move(oldSlots[i].data.first),
            std::move(oldSlots[i].data.second));
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
      if (!target[idx].occupied) {
        target[idx].data.first = std::move(key);
        target[idx].data.second = std::move(value);
        target[idx].psl = psl;
        target[idx].occupied = true;
        ++size_;
        return;
      }
      if (equal_(target[idx].data.first, key)) {
        target[idx].data.second = std::move(value);
        return;
      }
      if (psl > target[idx].psl) {
        std::swap(key, target[idx].data.first);
        std::swap(value, target[idx].data.second);
        std::swap(psl, target[idx].psl);
      }
      ++psl;
      idx = (idx + 1) % cap;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::Iterator::Iterator(
      Slot* current, Slot* end):
    current_(current),
    end_(end)
  {}

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::Iterator::skipEmpty()
  {
    while (current_ != end_ && !current_->occupied) {
      ++current_;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::value_type&
  HashTable< Key, Value, Hash, Equal >::Iterator::operator*() const
  {
    return current_->data;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::value_type*
  HashTable< Key, Value, Hash, Equal >::Iterator::operator->() const
  {
    return &current_->data;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::Iterator&
  HashTable< Key, Value, Hash, Equal >::Iterator::operator++()
  {
    ++current_;
    skipEmpty();
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::Iterator
  HashTable< Key, Value, Hash, Equal >::Iterator::operator++(int)
  {
    Iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::Iterator::operator==(
      const Iterator& rhs) const
  {
    return current_ == rhs.current_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::Iterator::operator!=(
      const Iterator& rhs) const
  {
    return current_ != rhs.current_;
  }

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::ConstIterator::ConstIterator(
      const Iterator& it):
    current_(it.current_),
    end_(it.end_)
  {}

  template< class Key, class Value, class Hash, class Equal >
  HashTable< Key, Value, Hash, Equal >::ConstIterator::ConstIterator(
      const Slot* current, const Slot* end):
    current_(current),
    end_(end)
  {}

  template< class Key, class Value, class Hash, class Equal >
  void HashTable< Key, Value, Hash, Equal >::ConstIterator::skipEmpty()
  {
    while (current_ != end_ && !current_->occupied) {
      ++current_;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename HashTable< Key, Value, Hash, Equal >::value_type&
  HashTable< Key, Value, Hash, Equal >::ConstIterator::operator*() const
  {
    return current_->data;
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename HashTable< Key, Value, Hash, Equal >::value_type*
  HashTable< Key, Value, Hash, Equal >::ConstIterator::operator->() const
  {
    return &current_->data;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::ConstIterator&
  HashTable< Key, Value, Hash, Equal >::ConstIterator::operator++()
  {
    ++current_;
    skipEmpty();
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::ConstIterator
  HashTable< Key, Value, Hash, Equal >::ConstIterator::operator++(int)
  {
    ConstIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::ConstIterator::operator==(
      const ConstIterator& rhs) const
  {
    return current_ == rhs.current_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool HashTable< Key, Value, Hash, Equal >::ConstIterator::operator!=(
      const ConstIterator& rhs) const
  {
    return current_ != rhs.current_;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::Iterator
  HashTable< Key, Value, Hash, Equal >::begin()
  {
    Iterator result(slots_, slots_ + capacity_);
    result.skipEmpty();
    return result;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::Iterator
  HashTable< Key, Value, Hash, Equal >::end()
  {
    Slot* stop = slots_ + capacity_;
    return Iterator(stop, stop);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::ConstIterator
  HashTable< Key, Value, Hash, Equal >::begin() const
  {
    ConstIterator result(slots_, slots_ + capacity_);
    result.skipEmpty();
    return result;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::ConstIterator
  HashTable< Key, Value, Hash, Equal >::end() const
  {
    const Slot* stop = slots_ + capacity_;
    return ConstIterator(stop, stop);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::ConstIterator
  HashTable< Key, Value, Hash, Equal >::cbegin() const
  {
    return begin();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename HashTable< Key, Value, Hash, Equal >::ConstIterator
  HashTable< Key, Value, Hash, Equal >::cend() const
  {
    return end();
  }
}

#endif
