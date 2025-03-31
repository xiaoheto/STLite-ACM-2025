#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <climits>
#include <cstddef>

#include "exceptions.hpp"

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template <typename T>
class vector {
   public:
    /**
     *
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
    /**
     * you can see RandomAccessIterator at CppReference for help.
     */
    class const_iterator;
    class iterator {
        // The following code is written for the C++ type_traits library.
        // Type traits is a C++ feature for describing certain properties of a
        // type. For instance, for an iterator, iterator::value_type is the type
        // that the iterator points to. STL algorithms and containers may use
        // these type_traits (e.g. the following typedef) to work properly. In
        // particular, without the following code,
        // @code{std::sort(iter, iter1);} would not compile.
        // See these websites for more information:
        // https://en.cppreference.com/w/cpp/header/type_traits
        // About value_type:
        // https://blog.csdn.net/u014299153/article/details/72419713 About
        // iterator_category: https://en.cppreference.com/w/cpp/iterator
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::output_iterator_tag;

       private:
        T *ptr;
        vector<T> *container;  // 指向T类型数组的指针
        /**
         *  add data members
         *   just add whatever you want.
         */
       public:
        iterator(T *p, vector<T> *con) : ptr(p), container(con) {
        }
        /**x
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator operator+(const int &n) const {
            return iterator(ptr + n, container);
            //
        }
        iterator operator-(const int &n) const {
            return iterator(ptr - n, container);
            //
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw
        // invaild_iterator.
        int operator-(const iterator &rhs) const {
            if (container != rhs.container) {
                throw invalid_iterator();
            }
            return ptr - rhs.ptr;
            //
        }
        iterator &operator+=(const int &n) {
            ptr += n;
            return *this;
            //
        }  // 修改现有对象
        iterator &operator-=(const int &n) {
            ptr -= n;
            return *this;
            //
        }
        /**
         *  iter++
         */
        iterator operator++(int) {
            iterator temp = *this;
            ptr += 1;
            return temp;
        }
        /**
         *  ++iter
         */
        iterator &operator++() {
            ptr += 1;
            return *this;
        }
        /**
         *  iter--
         */
        iterator operator--(int) {
            iterator temp = *this;
            ptr -= 1;
            return temp;
        }
        /**
         *  --iter
         */
        iterator &operator--() {
            ptr -= 1;
            return *this;
        }
        /**
         *  *it
         */
        T &operator*() const {
            return *ptr;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the
         * same memory address).
         */
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };
    /**
     *
     * has same function as iterator, just for a const object.
     */
    class const_iterator {
       public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::output_iterator_tag;

       private:
        T *ptr;
        const vector<T> *container;  // 指向T类型数组的指针
        /**/
       public:
        const_iterator(T *p, const vector<T> *con) : ptr(p), container(con) {
        }
        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        const_iterator operator+(const int &n) const {
            return const_iterator(ptr + n, container);
            //
        }
        const_iterator operator-(const int &n) const {
            return const_iterator(ptr - n, container);
            //
        }
        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw
        // invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            if (container != rhs.container) {
                throw invalid_iterator();
            }
            return ptr - rhs.ptr;
            //
        }
        const_iterator &operator+=(const int &n) {
            ptr += n;
            return *this;
            //
        }  // 修改现有对象
        const_iterator &operator-=(const int &n) {
            ptr -= n;
            return *this;
            //
        }
        /**
         *  iter++
         */
        const_iterator operator++(int) {
            const_iterator temp = *this;
            ptr += 1;
            return temp;
        }
        /**
         *  ++iter
         */
        const_iterator &operator++() {
            ptr += 1;
            return *this;
        }
        /**
         *  iter--
         */
        const_iterator operator--(int) {
            const_iterator temp = *this;
            ptr -= 1;
            return temp;
        }
        /**
         *  --iter
         */
        const_iterator &operator--() {
            ptr -= 1;
            return *this;
        }
        /**
         *  *it
         */
        T &operator*() const {
            return *ptr;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the
         * same memory address).
         */
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };

   private:
    T *data;          // T元素的动态数组
    size_t size_;     // 数组中元素个数
    size_t capacity;  // 数组的容量大小
    void resize(size_t new_capacity) {
        T *new_data =
            static_cast<T *>(::operator new(sizeof(T) * new_capacity));
        for (size_t i = 0; i < size_; ++i) {
            new (&new_data[i]) T(std::move(data[i]));
            data[i].~T();
        }
        ::operator delete(data);
        data = new_data;
        capacity = new_capacity;
    }

   public:
    /**
     *  Constructs
     * At least two: default constructor, copy constructor
     */
    vector() : data(nullptr), size_(0), capacity(0) {
    }
    vector(const vector &other)
        : data(nullptr), size_(other.size_), capacity(other.capacity) {
        data =
            static_cast<T *>(::operator new(sizeof(T) * capacity));  // 分配内存
        for (size_t i = 0; i < size_; ++i) {
            new (&data[i]) T(other.data[i]);
        }
    }
    /**
     *  Destructor
     */
    ~vector() {
        clear();
        ::operator delete(data);
    }
    /**
     *  Assignment operator
     */
    vector &operator=(const vector &other) {
        if (this == &other) {
            return *this;
        }
        clear();
        if (capacity < other.size_) {
            resize(other.size_);
        }
        for (size_t i = 0; i < other.size_; ++i) {
            new (&data[i]) T(other.data[i]);
        }
        size_ = other.size_;
        return *this;
    }

    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T &at(const size_t &pos) {
        if (pos > size_) {
            throw index_out_of_bound();
        }
        return data[pos];
    }
    const T &at(const size_t &pos) const {
        if (pos > size_) {
            throw index_out_of_bound();
        }
        return data[pos];
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T &operator[](const size_t &pos) {
        if (pos < 0 || pos >= size_) {
            throw index_out_of_bound();
        }
        return data[pos];
    }
    const T &operator[](const size_t &pos) const {
        if (pos < 0 || pos >= size_) {
            throw index_out_of_bound();
        }
        return data[pos];
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T &front() const {
        if (empty()) {
            throw container_is_empty();
        }
        return data[0];
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T &back() const {
        if (empty()) {
            throw container_is_empty();
        }
        return data[size_ - 1];
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
        return iterator(data, this);
    }
    const_iterator cbegin() const {
        return const_iterator(data, this);
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
        return iterator(data + size_, this);
    }
    const_iterator cend() const {
        return const_iterator(data + size_, this);
    }
    /**
     * checks whether the container is empty
     */
    bool empty() const {
        return size_ == 0;
    }
    /**
     * returns the number of elements
     */
    size_t size() const {
        return size_;
    }
    /**
     * clears the contents
     */
    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            data[i].~T();
        }
        size_ = 0;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T &value) {
        size_t index = pos - begin();
        if (size_ == capacity) {
            resize(capacity == 0 ? 1 : capacity * 10);
        }
        for (size_t i = size_; i > index; --i) {
            new (&data[i]) T(std::move(data[i - 1]));
            data[i - 1].~T();
        }
        new (&data[index]) T(value);
        ++size_;
        return iterator(data + index, this);
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size
     * because after inserting the size will increase 1.)
     */
    iterator insert(const size_t &ind, const T &value) {
        if (ind > size_) {
            throw index_out_of_bound();
        }
        return insert(begin() + ind, value);
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is
     * returned.
     */
    iterator erase(iterator pos) {
        size_t index = pos - begin();
        for (size_t i = index; i < size_ - 1; ++i) {
            data[i] = std::move(data[i + 1]);
        }
        data[size_ - 1].~T();
        --size_;
        return iterator(data + index, this);
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t &ind) {
        if (ind >= size_) {
            throw index_out_of_bound();
        }
        return erase(begin() + ind);
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T &value) {
        if (size_ == capacity) {
            resize(capacity == 0 ? 1 : capacity * 2);
        }
        new (&data[size_]) T(value);
        ++size_;
    }

    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        if (empty()) {
            throw container_is_empty();  // 如果容器为空，抛出异常
        }
        data[size_ - 1].~T();
        --size_;
        if (size_ < capacity / 4) {
			resize(capacity / 2);
		}
	}
};


}

#endif