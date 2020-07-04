#ifndef VECTOR_H
#define VECTOR_H

#include <cstdlib>
#include <utility>
#include <iostream>

template <typename T>
struct vector {
    typedef T *iterator;
    typedef T const *const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const &);                  // O(N) strong
    vector &operator=(vector const &other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T &operator[](size_t i);                // O(1) nothrow
    T const &operator[](size_t i) const;    // O(1) nothrow

    T *data();                              // O(1) nothrow
    T const *data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T &front();                             // O(1) nothrow
    T const &front() const;                 // O(1) nothrow

    T &back();                              // O(1) nothrow
    T const &back() const;                  // O(1) nothrow
    void push_back(T const &);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector &);                     // O(1) nothrow

    iterator begin();                       // O(1) nothrow
    iterator end();                         // O(1) nothrow

    const_iterator begin() const;           // O(1) nothrow
    const_iterator end() const;             // O(1) nothrow

    iterator insert(iterator pos, T const &); // O(N) weak
    iterator insert(const_iterator pos, T const &); // O(N) weak

    iterator erase(iterator pos);           // O(N) weak
    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(iterator first, iterator last); // O(N) weak
    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    T *data_;
    size_t size_;
    size_t capacity_;

    size_t calculate_capacity();
};

template<typename T>
vector<T>::vector() {
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename T>
vector<T>::vector(vector<T> const &other) {
    if (other.size_ != 0) {
        data_ = static_cast<T *> (operator new(other.size() * sizeof(T)));
    } else {
        data_ = nullptr;
    }
    size_t pos = 0;
    try {
        for (; pos < other.size(); pos++) {
            new(data_ + pos) T(other.data_[pos]);
        }
    } catch (...) {
        while (pos != 0) {
            data_[pos - 1].~T();
            pos--;
        }
        operator delete(data_);
        throw;
    }
    size_ = other.size_;
    capacity_ = size_;
}

template<typename T>
vector<T> &vector<T>::operator=(vector<T> const &other) {
    vector<T> result(other);
    swap(result);
    return *this;
}

template<typename T>
vector<T>::~vector() {
    for (size_t i = 0; i < size_; i++) {
        data_[i].~T();
    }
    operator delete(data_);
}

template<typename T>
T &vector<T>::operator[](size_t i) {
    return data_[i];
}

template<typename T>
T const &vector<T>::operator[](size_t i) const {
    return data_[i];
}

template<typename T>
T *vector<T>::data() {
    return data_;
}

template<typename T>
T const *vector<T>::data() const {
    return data_;
}

template<typename T>
size_t vector<T>::size() const {
    return size_;
}

template<typename T>
T &vector<T>::front() {
    return data_[0];
}

template<typename T>
T const &vector<T>::front() const {
    return data_[0];
}

template<typename T>
T &vector<T>::back() {
    return data_[size_ - 1];
}

template<typename T>
T const &vector<T>::back() const {
    return data_[size_ - 1];
}

template<typename T>
void vector<T>::push_back(T const &val) {
    //insert(end(), val);
    if (size_ == capacity_) {
        T tmp(val);
        reserve(calculate_capacity());
        new(data_ + size_) T(tmp);
    } else {
        new(data_ + size_) T(val);
    }
    size_++;
}

template<typename T>
void vector<T>::pop_back() {
    //erase(end() - 1);
    data_[--size_].~T();
}

template<typename T>
bool vector<T>::empty() const {
    return size_ == 0;
}

template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
void vector<T>::reserve(size_t how_much) {
    if (how_much > capacity_) {
        vector<T> tmp;
        tmp.data_ = static_cast<T *>(operator new (how_much * sizeof(T)));
        tmp.capacity_ = how_much;
        for (size_t i = 0; i < size_; i++) {
            tmp.push_back(data_[i]);
        }
        swap(tmp);
    }
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (size_ == capacity_) {
        return;
    }
    vector<T> tmp(*this);
    swap(tmp);
}

template<typename T>
void vector<T>::clear() {
    for (size_t i = 0; i < size_; i++) {
        data_[i].~T();
    }
    size_ = 0;
}

template<typename T>
void vector<T>::swap(vector &other) {
    std::swap(data_, other.data_);
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
}

template<typename T>
T *vector<T>::begin() {
    return data_;
}

template<typename T>
T *vector<T>::end() {
    return data_ + size_;
}

template<typename T>
T const *vector<T>::begin() const {
    return data_;
}

template<typename T>
T const *vector<T>::end() const {
    return data_ + size_;
}

template<typename T>
T *vector<T>::insert(iterator pos, T const &val) {
    size_t position = pos - data_;
    //T value = val;
    //if (pos != nullptr) {
    //    position = pos - data_;
    //} else {
    //    position = 0;
    //}
    //if (size_ >= capacity_) {
    //    reserve(calculate_capacity());
    //}
    //if (position == size_) {
    //    new(end()) T(value);
    //    size_++;
    //    return data_ + position;
    //}
    //size_t max_pos = size_ - 1;
    //new(end()) T(data_[max_pos]);
    //size_++;
    //while (position < max_pos) {
    //    data_[max_pos] = data_[max_pos - 1];
    //    max_pos--;
    //}
    //data_[position] = value;
    push_back(val);
    for (size_t i = size_ - 1; i > position; i--) {
        std::swap(data_[i], data_[i - 1]);
    }
    return data_ + position;
}

template<typename T>
T *vector<T>::erase(iterator pos) {
    size_t position = pos - data_;
    for (size_t i = position; i < size_ - 1; i++) {
        //data_[i] = data_[i + 1];
        std::swap(data_[i], data_[i + 1]);
    }
    //data_[size_ - 1].~T();
    //size_--;
    pop_back();
    return data_ + position;
}

template<typename T>
T *vector<T>::erase(iterator first, iterator last) {
    size_t diff = last - first;
    size_t first_pos = first - data_;
    for (size_t i = first_pos; i < size_ - diff; i++) {
        //data_[i] = data_[i + diff];
        std::swap(data_[i], data_[i + diff]);
    }
    for (size_t i = size_ - diff; i < size_; i++) {
        data_[i].~T();
    }
    size_ -= diff;
    return data_ + first_pos;
}

template<typename T>
size_t vector<T>::calculate_capacity() {
    return capacity_ == 0 ? 1 : 2 * capacity_;
}

#endif // !VECTOR_H
