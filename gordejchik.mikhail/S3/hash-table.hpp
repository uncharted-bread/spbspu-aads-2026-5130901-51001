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
}

#endif
