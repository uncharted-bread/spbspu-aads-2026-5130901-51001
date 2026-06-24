#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace gordejchik {
  template< class Key, class Value, class Hash, class Equal >
  class HashTable {
  public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair< Key, Value >;

    class Iterator;
    class ConstIterator;

    HashTable(size_t numBuckets, size_t bucketSize,
              const Hash& hash = Hash(),
              const Equal& equal = Equal());
    ~HashTable();
    HashTable(const HashTable& other);
    HashTable& operator=(const HashTable& other);
    HashTable(HashTable&& other);
    HashTable& operator=(HashTable&& other);

    void insert(const Key& key, const Value& value);
    Value& at(const Key& key);
    const Value& at(const Key& key) const;
    Value& operator[](const Key& key);
    bool contains(const Key& key) const;
    void erase(const Key& key);

    size_t size() const;
    bool empty() const;
    size_t bucketCount() const;
    size_t getBucketSize() const;
    void clear();
    void rehash(size_t newBucketCount);
    void swap(HashTable& other);

    Iterator begin();
    Iterator end();
    ConstIterator begin() const;
    ConstIterator end() const;
    ConstIterator cbegin() const;
    ConstIterator cend() const;

  private:
    struct Entry {
      value_type data_;
      bool occupied_;
    };

    Entry* data_;
    size_t numBuckets_;
    size_t bucketSize_;
    size_t size_;
    Hash hash_;
    Equal equal_;

    size_t totalCapacity() const;
    size_t homeStart(const Key& key) const;
    size_t spareStart() const;
    Entry* findEntry(const Key& key);
    const Entry* findEntry(const Key& key) const;
  };

  template< class Key, class Value, class Hash, class Equal >
  class HashTable< Key, Value, Hash, Equal >::Iterator {
    friend class HashTable;
  public:
    using value_type = std::pair< Key, Value >;

    value_type& operator*();
    value_type* operator->();
    Iterator& operator++();
    Iterator operator++(int);
    bool operator==(const Iterator& rhs) const;
    bool operator!=(const Iterator& rhs) const;

  private:
    Entry* current_;
    Entry* end_;

    Iterator(Entry* current, Entry* end);
    void skipEmpty();
  };

  template< class Key, class Value, class Hash, class Equal >
  class HashTable< Key, Value, Hash, Equal >::ConstIterator {
    friend class HashTable;
  public:
    using value_type = std::pair< Key, Value >;

    ConstIterator(const Iterator& it);
    const value_type& operator*() const;
    const value_type* operator->() const;
    ConstIterator& operator++();
    ConstIterator operator++(int);
    bool operator==(const ConstIterator& rhs) const;
    bool operator!=(const ConstIterator& rhs) const;

  private:
    const Entry* current_;
    const Entry* end_;

    ConstIterator(const Entry* current, const Entry* end);
    void skipEmpty();
  };

  template< class Key, class Value, class Hash, class Equal >
  gordejchik::HashTable< Key, Value, Hash, Equal >::HashTable(
      size_t numBuckets, size_t bucketSize,
      const Hash& hash, const Equal& equal):
    data_(nullptr),
    numBuckets_(numBuckets),
    bucketSize_(bucketSize),
    size_(0),
    hash_(hash),
    equal_(equal)
  {
    if (numBuckets_ == 0 || bucketSize_ == 0) {
      throw std::invalid_argument("Bucket count and size must be positive");
    }
    data_ = new Entry[totalCapacity()]();
  }

  template< class Key, class Value, class Hash, class Equal >
  gordejchik::HashTable< Key, Value, Hash, Equal >::~HashTable()
  {
    delete[] data_;
  }

  template< class Key, class Value, class Hash, class Equal >
  gordejchik::HashTable< Key, Value, Hash, Equal >::HashTable(const HashTable& other):
    data_(nullptr),
    numBuckets_(other.numBuckets_),
    bucketSize_(other.bucketSize_),
    size_(other.size_),
    hash_(other.hash_),
    equal_(other.equal_)
  {
    const size_t cap = totalCapacity();
    data_ = new Entry[cap]();
    for (size_t i = 0; i < cap; ++i) {
      data_[i] = other.data_[i];
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  gordejchik::HashTable< Key, Value, Hash, Equal >&
  gordejchik::HashTable< Key, Value, Hash, Equal >::operator=(const HashTable& other)
  {
    if (this != &other) {
      HashTable temp(other);
      swap(temp);
    }
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  gordejchik::HashTable< Key, Value, Hash, Equal >::HashTable(HashTable&& other):
    data_(other.data_),
    numBuckets_(other.numBuckets_),
    bucketSize_(other.bucketSize_),
    size_(other.size_),
    hash_(other.hash_),
    equal_(other.equal_)
  {
    other.data_ = nullptr;
    other.size_ = 0;
    other.numBuckets_ = 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  gordejchik::HashTable< Key, Value, Hash, Equal >&
  gordejchik::HashTable< Key, Value, Hash, Equal >::operator=(HashTable&& other)
  {
    if (this != &other) {
      swap(other);
    }
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  void gordejchik::HashTable< Key, Value, Hash, Equal >::swap(HashTable& other)
  {
    Entry* tmpData = data_;
    data_ = other.data_;
    other.data_ = tmpData;

    size_t tmp = numBuckets_;
    numBuckets_ = other.numBuckets_;
    other.numBuckets_ = tmp;

    tmp = bucketSize_;
    bucketSize_ = other.bucketSize_;
    other.bucketSize_ = tmp;

    tmp = size_;
    size_ = other.size_;
    other.size_ = tmp;
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t gordejchik::HashTable< Key, Value, Hash, Equal >::totalCapacity() const
  {
    return (numBuckets_ + 1) * bucketSize_;
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t gordejchik::HashTable< Key, Value, Hash, Equal >::homeStart(const Key& key) const
  {
    return (hash_(key) % numBuckets_) * bucketSize_;
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t gordejchik::HashTable< Key, Value, Hash, Equal >::spareStart() const
  {
    return numBuckets_ * bucketSize_;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::Entry*
  gordejchik::HashTable< Key, Value, Hash, Equal >::findEntry(const Key& key)
  {
    const size_t home = homeStart(key);
    for (size_t i = home; i < home + bucketSize_; ++i) {
      if (data_[i].occupied_ && equal_(data_[i].data_.first, key)) {
        return &data_[i];
      }
    }
    const size_t spare = spareStart();
    for (size_t i = spare; i < spare + bucketSize_; ++i) {
      if (data_[i].occupied_ && equal_(data_[i].data_.first, key)) {
        return &data_[i];
      }
    }
    return nullptr;
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename gordejchik::HashTable< Key, Value, Hash, Equal >::Entry*
  gordejchik::HashTable< Key, Value, Hash, Equal >::findEntry(const Key& key) const
  {
    const size_t home = homeStart(key);
    for (size_t i = home; i < home + bucketSize_; ++i) {
      if (data_[i].occupied_ && equal_(data_[i].data_.first, key)) {
        return &data_[i];
      }
    }
    const size_t spare = spareStart();
    for (size_t i = spare; i < spare + bucketSize_; ++i) {
      if (data_[i].occupied_ && equal_(data_[i].data_.first, key)) {
        return &data_[i];
      }
    }
    return nullptr;
  }

  template< class Key, class Value, class Hash, class Equal >
  void gordejchik::HashTable< Key, Value, Hash, Equal >::insert(
      const Key& key, const Value& value)
  {
    Entry* existing = findEntry(key);
    if (existing) {
      existing->data_.second = value;
      return;
    }
    const size_t home = homeStart(key);
    for (size_t i = home; i < home + bucketSize_; ++i) {
      if (!data_[i].occupied_) {
        data_[i].data_ = value_type(key, value);
        data_[i].occupied_ = true;
        ++size_;
        return;
      }
    }
    const size_t spare = spareStart();
    for (size_t i = spare; i < spare + bucketSize_; ++i) {
      if (!data_[i].occupied_) {
        data_[i].data_ = value_type(key, value);
        data_[i].occupied_ = true;
        ++size_;
        return;
      }
    }
    throw std::overflow_error("Hash table overflow");
  }

  template< class Key, class Value, class Hash, class Equal >
  Value& gordejchik::HashTable< Key, Value, Hash, Equal >::at(const Key& key)
  {
    Entry* entry = findEntry(key);
    if (!entry) {
      throw std::out_of_range("Key not found");
    }
    return entry->data_.second;
  }

  template< class Key, class Value, class Hash, class Equal >
  const Value& gordejchik::HashTable< Key, Value, Hash, Equal >::at(const Key& key) const
  {
    const Entry* entry = findEntry(key);
    if (!entry) {
      throw std::out_of_range("Key not found");
    }
    return entry->data_.second;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value& gordejchik::HashTable< Key, Value, Hash, Equal >::operator[](const Key& key)
  {
    Entry* entry = findEntry(key);
    if (entry) {
      return entry->data_.second;
    }
    insert(key, Value());
    return findEntry(key)->data_.second;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool gordejchik::HashTable< Key, Value, Hash, Equal >::contains(const Key& key) const
  {
    return findEntry(key) != nullptr;
  }

  template< class Key, class Value, class Hash, class Equal >
  void gordejchik::HashTable< Key, Value, Hash, Equal >::erase(const Key& key)
  {
    Entry* entry = findEntry(key);
    if (!entry) {
      throw std::out_of_range("Key not found in erase");
    }
    entry->data_ = value_type();
    entry->occupied_ = false;
    --size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t gordejchik::HashTable< Key, Value, Hash, Equal >::size() const
  {
    return size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool gordejchik::HashTable< Key, Value, Hash, Equal >::empty() const
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t gordejchik::HashTable< Key, Value, Hash, Equal >::bucketCount() const
  {
    return numBuckets_;
  }

  template< class Key, class Value, class Hash, class Equal >
  size_t gordejchik::HashTable< Key, Value, Hash, Equal >::getBucketSize() const
  {
    return bucketSize_;
  }

  template< class Key, class Value, class Hash, class Equal >
  void gordejchik::HashTable< Key, Value, Hash, Equal >::clear()
  {
    for (size_t i = 0; i < totalCapacity(); ++i) {
      if (data_[i].occupied_) {
        data_[i].data_ = value_type();
        data_[i].occupied_ = false;
      }
    }
    size_ = 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  void gordejchik::HashTable< Key, Value, Hash, Equal >::rehash(size_t newBucketCount)
  {
    HashTable temp(newBucketCount, bucketSize_, hash_, equal_);
    for (size_t i = 0; i < totalCapacity(); ++i) {
      if (data_[i].occupied_) {
        temp.insert(data_[i].data_.first, data_[i].data_.second);
      }
    }
    swap(temp);
  }

  template< class Key, class Value, class Hash, class Equal >
  gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator::Iterator(
      Entry* current, Entry* end):
    current_(current),
    end_(end)
  {}

  template< class Key, class Value, class Hash, class Equal >
  void gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator::skipEmpty()
  {
    while (current_ != end_ && !current_->occupied_) {
      ++current_;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::value_type&
  gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator::operator*()
  {
    return current_->data_;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::value_type*
  gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator::operator->()
  {
    return &current_->data_;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator&
  gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator::operator++()
  {
    ++current_;
    skipEmpty();
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator
  gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator::operator++(int)
  {
    Iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator::operator==(
      const Iterator& rhs) const
  {
    return current_ == rhs.current_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator::operator!=(
      const Iterator& rhs) const
  {
    return current_ != rhs.current_;
  }

  template< class Key, class Value, class Hash, class Equal >
  gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator::ConstIterator(
      const Entry* current, const Entry* end):
    current_(current),
    end_(end)
  {}

  template< class Key, class Value, class Hash, class Equal >
  gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator::ConstIterator(
      const Iterator& it):
    current_(it.current_),
    end_(it.end_)
  {}

  template< class Key, class Value, class Hash, class Equal >
  void gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator::skipEmpty()
  {
    while (current_ != end_ && !current_->occupied_) {
      ++current_;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename gordejchik::HashTable< Key, Value, Hash, Equal >::value_type&
  gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator::operator*() const
  {
    return current_->data_;
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename gordejchik::HashTable< Key, Value, Hash, Equal >::value_type*
  gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator::operator->() const
  {
    return &current_->data_;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator&
  gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator::operator++()
  {
    ++current_;
    skipEmpty();
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator
  gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator::operator++(int)
  {
    ConstIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator::operator==(
      const ConstIterator& rhs) const
  {
    return current_ == rhs.current_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator::operator!=(
      const ConstIterator& rhs) const
  {
    return current_ != rhs.current_;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator
  gordejchik::HashTable< Key, Value, Hash, Equal >::begin()
  {
    Iterator it(data_, data_ + totalCapacity());
    it.skipEmpty();
    return it;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::Iterator
  gordejchik::HashTable< Key, Value, Hash, Equal >::end()
  {
    Entry* e = data_ + totalCapacity();
    return Iterator(e, e);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator
  gordejchik::HashTable< Key, Value, Hash, Equal >::begin() const
  {
    ConstIterator it(data_, data_ + totalCapacity());
    it.skipEmpty();
    return it;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator
  gordejchik::HashTable< Key, Value, Hash, Equal >::end() const
  {
    const Entry* e = data_ + totalCapacity();
    return ConstIterator(e, e);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator
  gordejchik::HashTable< Key, Value, Hash, Equal >::cbegin() const
  {
    return begin();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename gordejchik::HashTable< Key, Value, Hash, Equal >::ConstIterator
  gordejchik::HashTable< Key, Value, Hash, Equal >::cend() const
  {
    return end();
  }
}

#endif
