#pragma once

#include <cmath>
#include <functional>
#include <memory>
#include <utility>
#include <type_traits>
#include <vector>
#include <limits>

namespace fefu
{

template<typename T>
//класс, выделяющий и управляющий памятью
class allocator {
 public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = typename std::add_lvalue_reference<T>::type;
  using const_reference = typename std::add_lvalue_reference<const T>::type;
  using value_type = T;

  allocator() noexcept : x(10) {};

  explicit allocator(int k): x(k){}

  allocator(const allocator& k) noexcept : x(k.x) {}

  template <class U>
  allocator(const allocator<U>& k) noexcept : x(k.x) {};

  ~allocator() = default;

  pointer allocate(size_type n) {
      auto p = ::operator new(n * sizeof(value_type));
      return static_cast<pointer>(p);
  }

  void deallocate(pointer p, size_type n) noexcept {
      if (p != nullptr) {
          ::operator delete(p, n * sizeof(value_type));
      }
  }

  int x;
};

template<typename ValueType>
class hash_map_const_iterator;

template<typename ValueType>
class hash_map_iterator {
 public:

  template<typename, typename, typename, typename, typename>
  friend class hash_map;

  template<typename>
  friend class hash_map_const_iterator;

  using iterator_category = std::forward_iterator_tag;
  using value_type = ValueType;
  using difference_type = std::ptrdiff_t;
  using reference = ValueType&;
  using pointer = ValueType*;

  hash_map_iterator() noexcept = default;
  hash_map_iterator(const hash_map_iterator& other) noexcept : start(other.start),
                                                               state(other.state), position(other.position) {}

  reference operator*() const {
      return *(start + position);
  }

  pointer operator->() const {
      return start + position;
  }

  // prefix ++
  hash_map_iterator& operator++() {
      while (true) {
          position++;

          if (position == state->size()) {
              break;
          }


          if(*(state->data() + position) == 1) {
              break;
          }
      }

      return *this;
  }

  // postfix ++
  hash_map_iterator operator++(int) {
      hash_map_iterator tmp = *this;

      while (true) {
          position++;

          if ((position) == state->size()) {
              break;
          }

          if(*(state->data() + position) == 1) {
              break;
          }
      }

      return tmp;
  }

  friend bool operator==(const hash_map_iterator<ValueType>& a, const hash_map_iterator<ValueType>& b) {
      if (a.state == b.state) {
          if (a.start == b.start) {
              if (a.position == b.position) {
                  return true;
              }
          }
      }
      return false;
  }

  friend bool operator!=(const hash_map_iterator<ValueType>& a, const hash_map_iterator<ValueType>& b) {
      return !(a == b);
  }

 private:
  const pointer start;
  const std::vector<char>* state;
  size_t position;
  hash_map_iterator(pointer p, std::vector<char> *st, size_t pos): start(p), state(st), position(pos) {}
  hash_map_iterator(const hash_map_const_iterator<ValueType>& other) noexcept :
      start(const_cast<pointer>(other.start)),
      state(const_cast<std::vector<char>*>(other.state)),
      position(other.position) {}
};

template<typename ValueType>
class hash_map_const_iterator {
// Shouldn't give non const references on value
 public:
  template<typename, typename, typename, typename, typename>
  friend class hash_map;

  template<typename>
  friend class hash_map_iterator;

  using iterator_category = std::forward_iterator_tag;
  using value_type = ValueType;
  using difference_type = std::ptrdiff_t;
  using reference = const ValueType&;
  using pointer = const ValueType*;

  hash_map_const_iterator() noexcept = default;
  hash_map_const_iterator(const hash_map_const_iterator& other) noexcept : start(other.start), state(other.state),
                                                                           position(other.position) {}
  hash_map_const_iterator(const hash_map_iterator<ValueType>& other) noexcept : start(other.start), state(other.state),
                                                                                position(other.position) {}

  reference operator*() const {
      return *(start + position);
  }

  pointer operator->() const {
      return start + position;
  }

  // prefix ++
  hash_map_const_iterator& operator++() {
      while (true) {
          position++;

          if ((position) == state->size()) {
              break;
          }

          if(*(state->data() + position) == 1) {
              break;
          }
      }
      return *this;
  }

  // postfix ++
  hash_map_const_iterator operator++(int) {
      hash_map_const_iterator tmp = *this;

      while (true) {
          position++;

          if ((position) == state->size()) {
              break;
          }

          if(*(state->data() + position) == 1) {
              break;
          }
      }
      return tmp;
  }

  friend bool operator==(const hash_map_const_iterator<ValueType>& a, const hash_map_const_iterator<ValueType>& b){
      if (a.state == b.state) {
          if (a.start == b.start) {
              if (a.position == b.position) {
                  return true;
              }
          }
      }
      return false;
  }
  friend bool operator!=(const hash_map_const_iterator<ValueType>& a, const hash_map_const_iterator<ValueType>& b) {
      return !(a == b);
  }

 private:
  pointer start;
  const std::vector<char>* state;
  size_t position;
  hash_map_const_iterator(const pointer p, const std::vector<char> *st, const size_t pos): start(p), state(st), position(pos) {}
};
template<typename K, typename T,
    typename Hash = std::hash<K>,
    typename Pred = std::equal_to<K>,
    typename Alloc = allocator<std::pair<const K, T>>>
class hash_map
{
 public:
  using key_type = K;
  using mapped_type = T;
  using hasher = Hash;
  using key_equal = Pred;
  using allocator_type = Alloc;
  using value_type = std::pair<const key_type, mapped_type>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = hash_map_iterator<value_type>;
  using const_iterator = hash_map_const_iterator<value_type>;
  using size_type = std::size_t;

  /// Default constructor.
  hash_map() = default;

  ~hash_map() {
      alloc.deallocate(data, state.size());
  }

  /**
   *  @brief  Default constructor creates no elements.
   *  @param n  Minimal initial number of buckets.
   */
  explicit hash_map(size_type n) :
      act_size(0), exist_size(0),
      data(alloc.allocate(n)),
      state(n, 0){}

  /**
   *  @brief  Builds an %hash_map from a range.
   *  @param  first  An input iterator.
   *  @param  last  An input iterator.
   *  @param  n  Minimal initial number of buckets.
   *
   *  Create an %hash_map consisting of copies of the elements from
   *  [first,last).  This is linear in N (where N is
   *  distance(first,last)).
   */
  template<typename InputIterator>
  hash_map(InputIterator first, InputIterator last,
           size_type n = 0) : hash_map(n) {
      insert(first, last);
  }

  /// Copy constructor.
  hash_map(const hash_map& other) : hash_map(1) {
      alloc = other.getAllocator();
      hash = other.hash_function();
      equal_key = other.key_eq();
      alloc.deallocate(data, bucket_count());
      data = alloc.allocate(other.bucket_count());
      state = other.state;
      for (size_type i = 0; i < state.size(); i++) {
          if (state[i] != 0) {
              new(data + i) value_type{other.data[i].first, other.data[i].second};
          }
      }
      act_size = other.act_size;
      exist_size = other.exist_size;
      maxload = other.maxload;
  }

  /// Move constructor.
  hash_map(hash_map&& other) : hash_map(1) {
      swap(other);
  }

  /**
   *  @brief Creates an %hash_map with no elements.
   *  @param a An allocator object.
   */

  explicit hash_map(const allocator_type& a) :
      act_size(0), exist_size(0),
      state(1, 0), alloc(a),
      data(alloc.allocate(1)) {}

  /*
  * @brief Copy constructor with allocator argument.
  * @param  uset  Input %hash_map to copy.
  * @param  a  An allocator object.
  */

  hash_map(const hash_map& umap,
           const allocator_type& a) : alloc(a) {
      hash = umap.hash_function();
      equal_key = umap.key_eq();
      data = alloc.allocate(umap.bucket_count());
      state = umap.state;
      for (size_type i = 0; i < state.size(); i++) {
          if (state[i] != 0) {
              new(data + i) value_type{umap.data[i].first, umap.data[i].second};
          }
      }
      act_size = umap.act_size;
      exist_size = umap.exist_size;
      maxload = umap.maxload;
  }

  /*
  *  @brief  Move constructor with allocator argument.
  *  @param  uset Input %hash_map to move.
  *  @param  a    An allocator object.
  */
  hash_map(hash_map&& umap,
           const allocator_type& a) : hash_map(1) {
      swap(umap);
      alloc = a;
  }

  /**
   *  @brief  Builds an %hash_map from an initializer_list.
   *  @param  l  An initializer_list.
   *  @param n  Minimal initial number of buckets.
   *
   *  Create an %hash_map consisting of copies of the elements in the
   *  list. This is linear in N (where N is @a l.size()).
   */

  hash_map(std::initializer_list<value_type> l,
           size_type n = 0) : hash_map(n) {
      insert(l);
  }

  ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Copy assignment operator.
  hash_map& operator=(const hash_map& other) {
      alloc = other.getAllocator();
      hash = other.hash_function();
      equal_key = other.key_eq();
      alloc.deallocate(data, bucket_count());
      data = alloc.allocate(other.bucket_count());
      state = other.state;
      for (size_type i = 0; i < state.size(); i++) {
          if (state[i] != 0) {
              new(data + i) value_type{other.data[i].first, other.data[i].second};
          }
      }
      act_size = other.act_size;
      exist_size = other.exist_size;
      maxload = other.maxload;
      return *this;
  }
  ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /// Move assignment operator.
  hash_map& operator=(hash_map&& h) {
      swap(h);
      return *this;
  }
  ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /**
   *  @brief  %hash_map list assignment operator.
   *  @param  l  An initializer_list.
   *
   *  This function fills an %hash_map with copies of the elements in
   *  the initializer list @a l.
   *
   *  Note that the assignment completely changes the %hash_map and
   *  that the resulting %hash_map's size is the same as the number
   *  of elements assigned.
   */

  ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  hash_map& operator=(std::initializer_list<value_type> l) {
      hash_map res(l.size());
      res.insert(l);
      *this = res;
      return *this;
  }
  ///~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  ///  Returns the allocator object used by the %hash_map.
  allocator_type getAllocator() const noexcept {
      return alloc;
  }

  // size and capacity:

  ///  Returns true if the %hash_map is empty.
  bool empty() const noexcept {
      return exist_size == 0;
  }

  ///  Returns the size of the %hash_map.
  size_type size() const noexcept {
      return exist_size;
  }

  ///  Returns the maximum size of the %hash_map.
  size_type max_size() const noexcept {
      return std::numeric_limits<size_type>::max();
  }

  // iterators.

  /**
   *  Returns a read/write iterator that points to the first element in the
   *  %hash_map.
   */
  iterator begin() noexcept {
      for (size_type i = 0; i < state.size(); i++) {
          if (state[i] == 1) {
              return iterator(data, &state, i);
          }
      }
      return end();
  }

  //@{
  /**
   *  Returns a read-only (constant) iterator that points to the first
   *  element in the %hash_map.
   */
  const_iterator begin() const noexcept {
      for (size_type i = 0; i < state.size(); i++) {
          if (state[i] == 1) {
              return const_iterator(data, &state, i);
          }
      }
      return cend();
  }

  const_iterator cbegin() const noexcept {
      for (size_type i = 0; i < state.size(); i++) {
          if (state[i] == 1) {
              return const_iterator(data, &state, i);
          }
      }
      return cend();
  }

  /**
   *  Returns a read/write iterator that points one past the last element in
   *  the %hash_map.
   */
  iterator end() noexcept {
      return iterator(data, &state, state.size());
  }

  //@{
  /**
   *  Returns a read-only (constant) iterator that points one past the last
   *  element in the %hash_map.
   */
  const_iterator end() const noexcept {
      return const_iterator(data, &state, state.size());
  }

  const_iterator cend() const noexcept {
      return const_iterator(data, &state, state.size());
  }
  //@}

  // modifiers.

  /**
   *  @brief Attempts to build and insert a std::pair into the
   *  %hash_map.
   *
   *  @param args  Arguments used to generate a new pair instance (see
   *	        std::piecewise_contruct for passing arguments to each
  *	        part of the pair constructor).
  *
  *  @return  A pair, of which the first element is an iterator that points
  *           to the possibly inserted pair, and the second is a bool that
  *           is true if the pair was actually inserted.
  *
  *  This function attempts to build and insert a (key, value) %pair into
  *  the %hash_map.
  *  An %hash_map relies on unique keys and thus a %pair is only
  *  inserted if its first element (the key) is not already present in the
  *  %hash_map.
  *
  *  Insertion requires amortized constant time.
  */

  template<typename... _Args>
  std::pair<iterator, bool> emplace(_Args&&... args) {
      auto x = value_type(std::forward<_Args>(args)...);
      return insert(x);
  }

  /**
   *  @brief Attempts to build and insert a std::pair into the
   *  %hash_map.
   *
   *  @param k    Key to use for finding a possibly existing pair in
   *                the hash_map.
   *  @param args  Arguments used to generate the .second for a
   *                new pair instance.
   *
   *  @return  A pair, of which the first element is an iterator that points
   *           to the possibly inserted pair, and the second is a bool that
   *           is true if the pair was actually inserted.
   *
   *  This function attempts to build and insert a (key, value) %pair into
   *  the %hash_map.
   *  An %hash_map relies on unique keys and thus a %pair is only
   *  inserted if its first element (the key) is not already present in the
   *  %hash_map.
   *  If a %pair is not inserted, this function has no effect.
   *
   *  Insertion requires amortized constant time.
   */
  template <typename... _Args>
  std::pair<iterator, bool> try_emplace(const key_type& k, _Args&&... args) {
      return insert({k, mapped_type(std::forward<_Args>(args)...) });
  }
  // move-capable overload
  template <typename... _Args>
  std::pair<iterator, bool> try_emplace(key_type&& k, _Args&&... args) {
      return insert({std::move(k), mapped_type(std::forward<_Args>(args)...) });
  }

  //@{
  /**
   *  @brief Attempts to insert a std::pair into the %hash_map.
   *  @param x Pair to be inserted (see std::make_pair for easy
   *	     creation of pairs).
  *
  *  @return  A pair, of which the first element is an iterator that
  *           points to the possibly inserted pair, and the second is
  *           a bool that is true if the pair was actually inserted.
  *
  *  This function attempts to insert a (key, value) %pair into the
  *  %hash_map. An %hash_map relies on unique keys and thus a
  *  %pair is only inserted if its first element (the key) is not already
  *  present in the %hash_map.
  *
  *  Insertion requires amortized constant time.
  */


  std::pair<iterator, bool> insert(const value_type& x) {
      _insert(x);
  }

  std::pair<iterator, bool> insert(value_type&& k) {
      _insert(std::move(k));
  }

  //@}

  /**
   *  @brief A template function that attempts to insert a range of
   *  elements.
   *  @param  first  Iterator pointing to the start of the range to be
   *                   inserted.
   *  @param  last  Iterator pointing to the end of the range.
   *
   *  Complexity similar to that of the range constructor.
   */
  template<typename _InputIterator>
  void insert(_InputIterator first, _InputIterator last) {
      for (auto i = first; i != last; ++i) {
          insert(*i);
      }
  }

  /**
   *  @brief Attempts to insert a list of elements into the %hash_map.
   *  @param  l  A std::initializer_list<value_type> of elements
   *               to be inserted.
   *
   *  Complexity similar to that of the range constructor.
   */
  void insert(std::initializer_list<value_type> l) {
      insert(l.begin(), l.end());
  }

  /**
   *  @brief Attempts to insert a std::pair into the %hash_map.
   *  @param k    Key to use for finding a possibly existing pair in
   *                the map.
   *  @param obj  Argument used to generate the .second for a pair
   *                instance.
   *
   *  @return  A pair, of which the first element is an iterator that
   *           points to the possibly inserted pair, and the second is
   *           a bool that is true if the pair was actually inserted.
   *
   *  This function attempts to insert a (key, value) %pair into the
   *  %hash_map. An %hash_map relies on unique keys and thus a
   *  %pair is only inserted if its first element (the key) is not already
   *  present in the %hash_map.
   *  If the %pair was already in the %hash_map, the .second of
   *  the %pair is assigned from obj.
   *
   *  Insertion requires amortized constant time.
   */
  template <typename _Obj>
  std::pair<iterator, bool> insert_or_assign(const key_type& k, _Obj&& obj) {
      auto d = mapped_type{obj};
      if (load_factor() >= max_load_factor() || state.size() == 0) {
          rehash((state.size() + 1) * 2);
      }

      auto p = findindex(k);
      if (p.first == -1) {
          return std::pair<iterator, bool>(end(), false);
      }
      if (p.second == 0) {
          new(data + p.first) value_type{k, d};
          exist_size++;
          act_size++;
      }
      else if (p.second == 1) {
          data[p.first].second = d;
      }
      else if (p.second == 2) {
          data[p.first].second = d;
          exist_size++;
      }
      state[p.first] = 1;
      return std::pair<iterator, bool>(iterator(data, &state, p.first), true);
  }

  // move-capable overload
  template <typename _Obj>
  std::pair<iterator, bool> insert_or_assign(key_type&& k, _Obj&& obj) {
      auto d = mapped_type{obj};
      if (load_factor() >= max_load_factor() || state.size() == 0) {
          rehash((state.size() + 1) * 2);
      }

      auto p = findindex(std::move(k));
      if (p.first == -1) {
          return std::pair<iterator, bool>(end(), false);
      }
      if (p.second == 0) {
          new(data + p.first) value_type{k, d};
          exist_size++;
          act_size++;
      }
      else if (p.second == 1) {
          data[p.first].second = d;
      }
      else if (p.second == 2) {
          data[p.first].second = d;
          exist_size++;
      }
      state[p.first] = 1;
      return std::pair<iterator, bool>(iterator(data, &state, p.first), true);
  }

  //@{
  /**
   *  @brief Erases an element from an %hash_map.
   *  @param  position  An iterator pointing to the element to be erased.
   *  @return An iterator pointing to the element immediately following
   *          @a position prior to the element being erased. If no such
   *          element exists, end() is returned.
   *
   *  This function erases an element, pointed to by the given iterator,
   *  from an %hash_map.
   *  Note that this function only erases the element, and that if the
   *  element is itself a pointer, the pointed-to memory is not touched in
   *  any way.  Managing the pointer is the user's responsibility.
   */
  iterator erase(const_iterator position) {
      if (bucket((*position).first) == state.size()) {
          return end();
      }
      state[bucket((*position).first)] = 2;
      exist_size--;
      return position++;
  }

  // LWG 2059.
  iterator erase(iterator position) {
      if (bucket((*position).first) == state.size()) {
          return end();
      }
      state[bucket((*position).first)] = 2;
      exist_size--;
      return position++;
  }
  //@}

  /**
   *  @brief Erases elements according to the provided key.
   *  @param  x  Key of element to be erased.
   *  @return  The number of elements erased.
   *
   *  This function erases all the elements located by the given key from
   *  an %hash_map. For an %hash_map the result of this function
   *  can only be 0 (not present) or 1 (present).
   *  Note that this function only erases the element, and that if the
   *  element is itself a pointer, the pointed-to memory is not touched in
   *  any way.  Managing the pointer is the user's responsibility.
   */
  size_type erase(const key_type& x) {
      if (bucket(x) == state.size()) {
          return 0;
      }
      state[bucket(x)] = 2;
      exist_size--;
      return 1;
  }

  /**
   *  @brief Erases a [first,last) range of elements from an
   *  %hash_map.
   *  @param  first  Iterator pointing to the start of the range to be
   *                  erased.
   *  @param last  Iterator pointing to the end of the range to
   *                be erased.
   *  @return The iterator @a last.
   *
   *  This function erases a sequence of elements from an %hash_map.
   *  Note that this function only erases the elements, and that if
   *  the element is itself a pointer, the pointed-to memory is not touched
   *  in any way.  Managing the pointer is the user's responsibility.
   */
  iterator erase(const_iterator first, const_iterator last) {
      for (auto i = first; i != last; i++) {
          erase(i);
      }
      return (iterator)last;
  }

  /**
   *  Erases all elements in an %hash_map.
   *  Note that this function only erases the elements, and that if the
   *  elements themselves are pointers, the pointed-to memory is not touched
   *  in any way.  Managing the pointer is the user's responsibility.
   */
  void clear() noexcept {

      state.assign(state.size(), 0);
      act_size = 0;
      exist_size = 0;
  }

  /**
   *  @brief  Swaps data with another %hash_map.
   *  @param  x  An %hash_map of the same element and allocator
   *  types.
   *
   *  This exchanges the elements between two %hash_map in constant
   *  time.
   *  Note that the global std::swap() function is specialized such that
   *  std::swap(m1,m2) will feed to this function.
   */
  void swap(hash_map& x) {
      using std::swap;
      swap(data, x.data);
      swap(state, x.state);
      swap(act_size, x.act_size);
      swap(exist_size, x.exist_size);
      swap(maxload, x.maxload);
      swap(equal_key, x.equal_key);
      swap(hash, x.hash);
      swap(alloc, x.alloc);
  }

  template<typename _H2, typename _P2>
  void merge(hash_map<K, T, _H2, _P2, Alloc>& source) {
      rehash(bucket_count() + source.bucket_count());
      insert(source.begin(), source.end());
  }

  template<typename _H2, typename _P2>
  void merge(hash_map<K, T, _H2, _P2, Alloc>&& source) {
      auto b = std::move(source);
      rehash(bucket_count() + b.bucket_count());
      insert(b.begin(), b.end());
  }

  // observers.

  ///  Returns the hash functor object with which the %hash_map was
  ///  constructed.
  Hash hash_function() const {
      return hash;
  }

  ///  Returns the key comparison object with which the %hash_map was
  ///  constructed.
  Pred key_eq() const {
      return equal_key;
  }

  // lookup.

  //@{
  /**
   *  @brief Tries to locate an element in an %hash_map.
   *  @param  x  Key to be located.
   *  @return  Iterator pointing to sought-after element, or end() if not
   *           found.
   *
   *  This function takes a key and tries to locate the element with which
   *  the key matches.  If successful the function returns an iterator
   *  pointing to the sought after element.  If unsuccessful it returns the
   *  past-the-end ( @c end() ) iterator.
   */
  iterator find(const key_type& x) {
      auto p = findindex(x);
      if (p.second == 1) {
          return iterator(data, &state, p.first);
      }
      return end();
  }

  const_iterator find(const key_type& x) const {
      auto p = findindex(x);
      if (p.second == 1) {
          return const_iterator(data, &state, p.first);
      }
      return cend();
  }
  //@}

  /**
   *  @brief  Finds the number of elements.
   *  @param  x  Key to count.
   *  @return  Number of elements with specified key.
   *
   *  This function only makes sense for %unordered_multimap; for
   *  %hash_map the result will either be 0 (not present) or 1
   *  (present).
   */
  size_type count(const key_type& x) const {
      return contains(x) ? 1 : 0;
  }

  /**
   *  @brief  Finds whether an element with the given key exists.
   *  @param  x  Key of elements to be located.
   *  @return  True if there is any element with the specified key.
   */
  bool contains(const key_type& x) const {
      return bucket(x) != state.size();
  }

  //@{
  /**
   *  @brief  Subscript ( @c [] ) access to %hash_map data.
   *  @param  k  The key for which data should be retrieved.
   *  @return  A reference to the data of the (key,data) %pair.
   *
   *  Allows for easy lookup with the subscript ( @c [] )operator.  Returns
   *  data associated with the key specified in subscript.  If the key does
   *  not exist, a pair with that key is created using default values, which
   *  is then returned.
   *
   *  Lookup requires constant time.
   */
  mapped_type& operator[](const key_type& k) {
      return _operator(k);
  }

  mapped_type& operator[](key_type&& k) {
      return _operator(std::move(k));
  }
  //@}

  //@{
  /**
   *  @brief  Access to %hash_map data.
   *  @param  k  The key for which data should be retrieved.
   *  @return  A reference to the data whose key is equal to @a k, if
   *           such a data is present in the %hash_map.
   *  @throw  std::out_of_range  If no such data is present.
   */

  mapped_type& at(const key_type& k) {
      auto p = findindex(k);
      if (p.second == 1) {
          return data[p.first].second;
      }
      throw std::out_of_range("");
  }

  const mapped_type& at(const key_type& k) const {
      auto p = findindex(std::move(k));
      if (p.second == 1) {
          return data[p.first].second;
      }
      throw std::out_of_range("");
  }
  //@}

  // bucket interface.

  /// Returns the number of buckets of the %hash_map.
  size_type bucket_count() const noexcept {
      return state.size();
  }

  /*
  * @brief  Returns the bucket index of a given element.
  * @param  _K  A key instance.
  * @return  The key bucket index.
  */
  size_type bucket(const key_type& _K) const {
      auto p = findindex(_K);
      if (p.second == 1) {
          return p.first;
      }
      return state.size();
  }

  // hash policy.

  /// Returns the average number of elements per bucket.
  float load_factor() const noexcept {
      if (state.size() == 0) {
          return 0;
      }
      return static_cast<float>(act_size / state.size());
  }

  /// Returns a positive number that the %hash_map tries to keep the
  /// load factor less than or equal to.
  float max_load_factor() const noexcept {
      return maxload;
  }

  /**
   *  @brief  Change the %hash_map maximum load factor.
   *  @param  z The new maximum load factor.
   */
  void max_load_factor(float z) {
      maxload = z;
  }

  /**
   *  @brief  May rehash the %hash_map.
   *  @param  n The new number of buckets.
   *
   *  Rehash will occur only if the new number of buckets respect the
   *  %hash_map maximum load factor.
   */
  void rehash(size_type n) {
      hash_map res(n);
      res.insert(begin(), end());
      *this = res;
  }

  /**
   *  @brief  Prepare the %hash_map for a specified number of
   *          elements.
   *  @param  n Number of elements required.
   *
   *  Same as rehash(ceil(n / max_load_factor())).
   */

  void reserve(size_type n) {
      hash_map res(std::ceil(static_cast<float>(n) / static_cast<float>(max_load_factor())));
      res.insert(begin(), end());
      *this = res;
  }

  bool operator==(const hash_map& other) const {
      if (alloc.x != other.alloc.x) {
          return false;
      }
      if (state.size() != other.state.size() || exist_size != other.exist_size) {
          return false;
      }
      for (size_t i = 0; i < state.size(); i++) {
          if (state[i] == 1 && state[i] != other.state[i]) {
              return false;
          }
          if (state[i] == 1 && other.state[i] == 1) {
              if (!equal_key(data[i].first, other.data[i].first)) {
                  return false;
              }
              if (data[i].second != other.data[i].second) {
                  return false;
              }
          }
      }
      return true;
  }

 private:
  allocator_type alloc;
  key_equal equal_key;
  hasher hash;
  value_type * data;
  std::vector<char> state;
  size_type act_size = 0;
  size_type exist_size = 0;
  float maxload = 1.0;

  std::pair<int, int> findindex(const key_type& k) const {
      if (state.size() == 0) {
          return std::pair<int, int>(-1, -1);
      }
      const size_type n = hash(k);
      const size_type index = n % state.size();

      size_type i = index;
      while (true) {
          if (state[i] == 0) {
              return std::pair<int, int>(i, 0);
          }
          if (equal_key(data[i].first, k)) {
              if (state[i] == 1) {
                  return std::pair<int, int>(i, 1);
              }
              else {
                  return std::pair<int, int>(i, 2);
              }
          }
          i++;
          if (i == state.size()) {
              i = 0;
          }
          if (i == index) {
              break;
          }
      }
      return std::pair<int, int>(-1, -1);
  }

  template<typename DDD>
  std::pair<iterator, bool> _insert(DDD&& x) {
      if (load_factor() >= max_load_factor() || state.size() == 0) {
          rehash((state.size() + 1) * 2);
      }

      auto p = findindex(x.first);
      if (p.first == -1) {
          return std::pair<iterator, bool>(end(), false);
      }
      if (p.second == 1) {
          return std::pair<iterator, bool>(iterator(data, &state, p.first), false);
      }
      if (p.second == 0) {
          new(data + p.first) value_type{std::forward<DDD>(x)};
          act_size++;
      }
      else if (p.second == 2) {
          data[p.first].second = std::forward<DDD>(x).second;
      }
      exist_size++;
      state[p.first] = 1;
      return std::pair<iterator, bool>(iterator(data, &state, p.first), true);
  }

  template <typename EEE>
  mapped_type& _operator(EEE&& k) {
      if (load_factor() >= max_load_factor() || state.size() == 0) {
          rehash((state.size() + 1) * 2);
      }
      auto p = findindex(std::forward<EEE>(k));
      if (p.second == 1) {
          return data[p.first].second;
      }
      if (p.second == 0) {
          new(data + p.first) value_type{std::forward<EEE>(k), mapped_type{}};
          state[p.first] = 1;
          act_size++;
          exist_size++;
          return data[p.first].second;
      }
      if (p.second == 2) {
          data[p.first].second = mapped_type{};
          state[p.first] = 1;
          exist_size++;
          return data[p.first].second;
      }
  }
};

} // namespace fefu