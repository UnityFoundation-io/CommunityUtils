#ifndef HSDS_COMMON_CONTAINER_H
#define HSDS_COMMON_CONTAINER_H

// Copyright 2023 CommunityUtils Authors

#include <map>
#include <string>
#include <vector>

template <class T>
class Container {
public:
  typedef std::vector<T> ListType;
  // TODO: Could I use an OpenDDS type here?
  typedef std::pair<std::string, std::string> KeyType;
  typedef std::map<KeyType, size_t> MapType;
  typedef typename ListType::const_iterator const_iterator;

  const_iterator find(const KeyType& key) const
  {
    MapType::const_iterator pos = map_.find(key);
    if (pos == map_.end()) {
      return list_.end();
    }

    const_iterator pos2 = list_.begin();
    std::advance(pos2, pos->second);
    return pos2;
  }

  const_iterator begin() const { return list_.begin(); }
  const_iterator end() const { return list_.end(); }

  void insert(const T& value)
  {
    const KeyType key(value.dpmgid(), value.id());
    MapType::const_iterator pos = map_.find(key);
    if (pos != map_.end()) {
      list_[pos->second] = value;
    }

    // TODO(sonndinh): Should these 2 lines be put into an else block
    // (when pos == map_.end())?
    map_[key] = list_.size();
    list_.push_back(value);
  }

  void erase(const T& value)
  {
    const KeyType key(value.dpmgid(), value.id());
    erase(key);
  }

  void erase(const KeyType& key1)
  {
    MapType::iterator pos1 = map_.find(key1);
    if (pos1 == map_.end()) {
      return;
    }

    // Find the end element.
    const KeyType key2(list_.back().dpmgid(), list_.back().id());
    MapType::iterator pos2 = map_.find(key2);

    // Swap.
    std::swap(list_[pos1->second], list_[pos2->second]);
    std::swap(pos1->second, pos2->second);

    // Remove the end element.
    map_.erase(pos1);
    list_.pop_back();
  }

  void clear()
  {
    list_.clear();
    map_.clear();
  }

  void swap(Container& other)
  {
    if (this != &other) {
      std::swap(list_, other.list_);
      std::swap(map_, other.map_);
    }
  }

  size_t size() const { return list_.size(); }

private:
  ListType list_;
  // Pair is dpmgid, id.
  MapType map_;
};

#endif // HSDS_COMMON_CONTAINER_H
