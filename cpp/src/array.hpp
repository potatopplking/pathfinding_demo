#include "log.hpp"
#include <expected>
#include <iostream>
#include <stack>
#include <stdexcept>

namespace array { // TODO rename to container or something

template <typename U>
concept Deletable = requires(U u) {
  { delete u } -> std::same_as<void>;
};

//
// Unordered dynamic container with contiguous memory
// with fast deleting and adding
//
template <typename T> class Pool {

  // Pair type and helper functions

  using pair_t = std::pair<bool, T>;

  static T &get_item(pair_t &X) { return std::get<1>(X); }

  static bool is_valid(const pair_t &X) { return std::get<0>(X); }

  static bool &validity(pair_t &X) { return std::get<0>(X); }

public:
  // Forward declaration for Iterator
  class Iterator;

  Pool(size_t prealloc_size = 16) {
    LOG_DEBUG(".");
    Realloc(prealloc_size);
  }

  Pool(const Pool &p) = delete;
  Pool(Pool &&p) = delete;

  ~Pool() {
    LOG_DEBUG(".");
    delete[] m_Pool;
  }

  template <typename U> void Add(U &&u) {
    EnsureAddCapacity(1);
    auto idx = m_FreeIdx.top();
    get_item(m_Pool[idx]) = std::forward<U>(u);
    validity(m_Pool[idx]) = true;
    m_FreeIdx.pop();
    m_Size++;
  }

  void Remove(Iterator &it) {
    // Calculate the index from the iterator's pointer
    size_t index = it.ptr - m_Pool;

    // Validate the iterator points to a valid element
    if (index >= m_Capacity || !is_valid(m_Pool[index])) {
      throw std::logic_error("Invalid iterator for removal");
    }

    // Delete if deletable
    if constexpr (Deletable<T>)
      delete get_item(m_Pool[index]);

    // Mark as invalid and add to free list
    validity(m_Pool[index]) = false;
    m_FreeIdx.push(index);
    m_Size--;

    // Advance iterator to next valid element
    it.ptr++;
    it.iter_until_valid();
  }

  void Remove(size_t index) {
    index_check(index);
    if constexpr (Deletable<T>)
      delete get_item(m_Pool[index]);
    validity(m_Pool[index]) = false;
    m_Size--;
  }

  T &operator[](size_t index) {
    index_check(index);
    return get_item(m_Pool[index]);
  }

  const T &operator[](size_t index) const {
    index_check(index);
    return get_item(m_Pool[index]);
  }

  friend std::ostream &operator<<(std::ostream &os, const Pool &obj) {
    os << "Pool( Size: " << obj.m_Size << ", capacity: " << obj.m_Capacity
       << "\n";
    for (size_t i = 0; i < obj.m_Capacity; i++) {
      os << "\t" << (std::get<bool>(obj.m_Pool[i]) ? "VALID" : "INVALID")
         << "\n";
    }
    os << "\n)";
    return os;
  }

  auto Size() const { return m_Size; }

  auto Capacity() const { return m_Capacity; }

  // Iterating related
  class Iterator {
    friend class Pool; // Allow Pool to access private members

  public:
    Iterator(pair_t *start, pair_t *stop) : ptr(start), end_ptr(stop) {
      iter_until_valid();
    }

    T &operator*() const { return get_item(*ptr); }

    Iterator &operator++() {
      if (ptr != end_ptr) {
        ptr++;
        iter_until_valid();
      }
      return *this;
    }

    bool operator!=(const Iterator &other) const { return ptr != other.ptr; }

    bool operator==(const Iterator &other) const { return ptr == other.ptr; }

  private:
    pair_t *ptr = nullptr;
    pair_t *end_ptr = nullptr;

    void iter_until_valid() {
      while (ptr < end_ptr && !is_valid(*ptr)) {
        ptr++;
      }
    }
  };

  Iterator begin() { return Iterator(m_Pool, m_Pool + m_Capacity); }
  Iterator end() { return Iterator(m_Pool + m_Capacity, m_Pool + m_Capacity); }

private:
  pair_t *m_Pool = nullptr;
  // TODO use unique_ptr
  std::stack<size_t> m_FreeIdx;

  size_t m_Capacity = 0;
  size_t m_Size = 0;

  void index_check(size_t index) const {
    if (index >= m_Capacity) {
      throw std::out_of_range("Out of range");
    }
    if (!is_valid(m_Pool[index])) {
      throw std::logic_error("Invalid item");
    }
  }

  void Realloc(size_t requested_capacity) {
    // Calculate new capacity
    size_t new_capacity = m_Capacity > 0 ? m_Capacity : 1;
    while (new_capacity < requested_capacity) {
      new_capacity *= 2;
    }
    LOG_DEBUG("Realloc from ", m_Capacity, " to ", new_capacity);
    // Alloc new pool and copy from old pool
    pair_t *new_pool = new pair_t[new_capacity];
    size_t new_idx = 0;
    size_t old_capacity = m_Capacity;
    for (size_t old_idx = 0; old_idx < old_capacity; old_idx++) {
      if (is_valid(m_Pool[old_idx])) {
        new_pool[new_idx++] = std::move(m_Pool[old_idx]);
      }
    }
    // Remaining new items are all free
    while (new_idx < new_capacity) {
      m_FreeIdx.push(new_idx);
      validity(new_pool[new_idx++]) = false;
    }
    // Swap and free the old pool
    std::swap(m_Pool, new_pool);
    delete[] new_pool;
    // Update state variables
    m_Capacity = new_capacity;
    // m_Size stays the same
  }

  void EnsureAddCapacity(size_t additional_elements) {
    if (m_Size + additional_elements > m_Capacity) {
      Realloc(m_Size + additional_elements);
    }
  }
};

} /* namespace array */
