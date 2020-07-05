#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <vector>
#include <functional>
#include <utility>
#include <algorithm>
#include <cassert>
#include <iostream>

class big_integer
{
public:
    big_integer();
    big_integer(big_integer const&) = default;
    big_integer(int);
    big_integer(uint32_t);
    explicit big_integer(std::string const&);
    ~big_integer() = default;

    big_integer& operator=(big_integer const&);

    big_integer& operator+=(big_integer const&);
    big_integer& operator-=(big_integer const&);
    big_integer& operator*=(big_integer const&);
    big_integer& operator/=(big_integer const&);
    big_integer& operator%=(big_integer const&);

    big_integer& operator&=(big_integer const&);
    big_integer& operator|=(big_integer const&);
    big_integer& operator^=(big_integer const&);

    big_integer& operator<<=(int);
    big_integer& operator>>=(int);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer& operator--();

    friend bool operator==(big_integer const&, big_integer const&);
    friend bool operator!=(big_integer const&, big_integer const&);
    friend bool operator<(big_integer const&, big_integer const&);
    friend bool operator>(big_integer const&, big_integer const&);
    friend bool operator<=(big_integer const&, big_integer const&);
    friend bool operator>=(big_integer const&, big_integer const&);

    friend big_integer operator+(big_integer, big_integer const&);
    friend big_integer operator-(big_integer, big_integer const&);
    friend big_integer operator*(big_integer, big_integer const&);
    friend big_integer operator/(big_integer, big_integer const&);

    friend big_integer operator<<(big_integer, int);
    friend big_integer operator>>(big_integer, int);

    friend std::string to_string(big_integer const&);

private:
    void swap(big_integer &);
    void delete_zeros();
    void addition_to_two(size_t);

    friend big_integer abstract_bitwise_operation(big_integer, big_integer const&, std::function<uint32_t(uint32_t, uint32_t)>);
    friend bool smaller(big_integer const&, big_integer const&, size_t, size_t);
    friend void difference(big_integer &, big_integer const&, size_t, size_t);
    friend big_integer short_div(big_integer const&, uint32_t);
    friend uint32_t trial(uint64_t, uint64_t, uint64_t);

    std::vector<uint32_t> digits;
    bool sign;
};

big_integer operator%(big_integer, big_integer const&);

big_integer operator&(big_integer, big_integer const&);
big_integer operator|(big_integer, big_integer const&);
big_integer operator^(big_integer, big_integer const&);

std::string to_string(big_integer const&);
std::ostream &operator<<(std::ostream &, big_integer const &);

#endif // BIG_INTEGER_H
