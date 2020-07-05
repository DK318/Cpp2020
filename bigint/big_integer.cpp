#include "big_integer.h"

big_integer::big_integer() : digits({0}), sign(false) {}

big_integer::big_integer(int x) : sign(x < 0)
{
    digits.push_back(x < 0 ? (uint32_t)(-x) : (uint32_t)x);
}

big_integer::big_integer(const std::string &str) : big_integer(0)
{
    size_t pos = (str[0] == '+' || str[0] == '-' ? 1 : 0);
    for (size_t i = pos; i < str.length(); i++) {
        if ('0' <= str[i] && str[i] <= '9') {
            *this *= 10;
            *this += (str[i] - '0');
        } else {
            throw std::invalid_argument("String is not a number");
        }
    }
    if (digits.size() == 1 && digits[0] == 0) {
        sign = false;
    } else {
        sign = (str[0] == '-');
    }
}

big_integer::big_integer(uint32_t x) : sign(false)
{
    digits.push_back(x);
}

big_integer &big_integer::operator=(const big_integer &second)
{
    big_integer tmp(second);
    swap(tmp);
    return *this;
}

big_integer &big_integer::operator+=(const big_integer &x)
{
    return *this = *this + x;
}

big_integer &big_integer::operator-=(const big_integer &x)
{
    return *this = *this - x;
}

big_integer &big_integer::operator*=(const big_integer &x)
{
    return *this = *this * x;
}

big_integer &big_integer::operator/=(const big_integer &x)
{
    return *this = *this / x;
}

big_integer &big_integer::operator%=(const big_integer &x)
{
    return *this = *this % x;
}

big_integer &big_integer::operator&=(const big_integer &x)
{
    return *this = *this & x;
}

big_integer &big_integer::operator|=(const big_integer &x)
{
    return *this = *this | x;
}

big_integer &big_integer::operator^=(const big_integer &x)
{
    return *this = *this ^ x;
}

big_integer &big_integer::operator<<=(int x)
{
    return *this = *this << x;
}

big_integer &big_integer::operator>>=(int x)
{
    return *this = *this >> x;
}

big_integer big_integer::operator+() const
{
    return big_integer(*this);
}

big_integer operator-(big_integer a, const big_integer &b)
{
    if (a.sign != b.sign)
        return (a.sign ? -(-a + b) : a + (-b));
    if (a.sign)
        return -b - (-a);
    if (a < b)
        return -(b - a);
    bool carry = false;
    int64_t subtract;
    big_integer result;
    result.digits.resize(a.digits.size());
    for (size_t i = 0; i < a.digits.size(); i++) {
        subtract = a.digits[i];
        if (carry)
            subtract--;
        if (i < b.digits.size())
            subtract -= b.digits[i];
        carry = (subtract < 0);
        result.digits[i] = (uint32_t)subtract;
    }
    result.delete_zeros();
    return result;
}

big_integer big_integer::operator-() const
{
    if (digits.size() == 1 && digits[0] == 0)
        return *this;
    big_integer res(*this);
    res.sign ^= true;
    return res;
}

uint32_t trial(uint64_t a, uint64_t b, uint64_t c)
{
    return std::min((uint32_t)(((a << 32) + b) / c), UINT32_MAX);
}

big_integer short_div(const big_integer &a, uint32_t b)
{
    big_integer result;
    uint64_t carry = 0;
    for (int i = (int)a.digits.size() - 1; i >= 0; i--) {
        uint64_t cur = (carry << 32) | a.digits[i];
        result.digits.push_back((uint32_t)(cur / b));
        carry = cur % b;
    }
    std::reverse(result.digits.begin(), result.digits.end());
    result.delete_zeros();
    return result;
}

void difference(big_integer &a, const big_integer &b, size_t k, size_t m)
{
    bool carry = false;
    for (size_t i = 0; i < m; i++) {
        uint32_t first = a.digits[i + k - 1];
        uint32_t second = (i < b.digits.size() ? b.digits[i] : 0);
        uint64_t cur = (int64_t)first - second - carry;
        carry = (second + carry > first);
        cur &= UINT32_MAX;
        a.digits[i + k - 1] = (uint32_t)cur;
    }
}

bool smaller(const big_integer &a, const big_integer &b, size_t k, size_t m)
{
    for (size_t i = m; i != 0; i--) {
        uint32_t value = (i < b.digits.size() ? b.digits[i] : 0);
        if (a.digits[i + k - 1] != value) {
            return a.digits[i + k - 1] < value;
        }
    }
    return false;
}

big_integer operator/(big_integer a, const big_integer &b)
{
    big_integer first = a, second = b;
    big_integer result, dq;
    first.sign = second.sign = false;
    if (first < second)
        return 0;
    if (second.digits.size() == 1) {
        result = short_div(first, second.digits[0]);
        result.sign = a.sign ^ b.sign;
        return result;
    }
    uint32_t factor = (((uint64_t)1 << 32)) / (second.digits.back() + 1);
    first *= factor;
    second *= factor;
    first.digits.push_back(0);
    size_t n = first.digits.size(), m = second.digits.size();
    result.digits.resize(n - m);
    for (size_t i = n - m; i != 0; i--) {
        uint32_t qt = trial(first.digits[i + m - 1], first.digits[i + m - 2], second.digits[m - 1]);
        dq = second * qt;
        while (qt != 0 && smaller(first, dq, i, m)) {
            qt--;
            dq -= second;
        }
        result.digits[i - 1] = qt;
        difference(first, dq, i, m);
    }
    result.delete_zeros();
    result.sign = a.sign ^ b.sign;
    return result;
}

big_integer operator>>(big_integer a, int shift)
{
    for (int i = 0; i < (shift / 32) && (a.digits.size() != 0); i++)
        a.digits.pop_back();
    shift %= 32;
    if (a.sign)
        a -= ((uint32_t)1 << shift) - 1;
    a /= (uint32_t)1 << shift;
    return a;
}

big_integer operator*(big_integer a, const big_integer &b)
{
    size_t sz = a.digits.size() + b.digits.size();
    big_integer result;
    result.digits.resize(sz);
    for (size_t i = 0; i < a.digits.size(); i++) {
        uint32_t carry = 0;
        for (size_t j = 0; j < b.digits.size() || carry; j++) {
            uint64_t mult = result.digits[i + j] + (uint64_t) a.digits[i] * (j < b.digits.size() ? b.digits[j] : 0) + carry;
            result.digits[i + j] = (uint32_t)(mult & UINT32_MAX);
            carry = (uint32_t)(mult >> 32);
        }
    }
    result.sign = a.sign ^ b.sign;
    result.delete_zeros();
    return result;
}

big_integer big_integer::operator~() const
{
    return -(*this) - 1;
}

big_integer &big_integer::operator++()
{
    return (*this += 1);
}

big_integer &big_integer::operator--()
{
    return (*this -= 1);
}

std::string to_string(const big_integer &x)
{
    if (x == 0)
        return "0";
    std::string result;
    big_integer tmp(x);
    while (tmp != 0) {
        result += char((tmp % 10).digits[0] + '0');
        tmp /= 10;
    }
    if (x.sign)
        result += "-";
    std::reverse(result.begin(), result.end());
    return result;
}

bool operator>=(const big_integer &a, const big_integer &b)
{
    return !(a < b);
}

bool operator<=(const big_integer &a, const big_integer &b)
{
    return !(a > b);
}

bool operator>(const big_integer &a, const big_integer &b)
{
    return b < a;
}

bool operator<(const big_integer &a, const big_integer &b)
{
    if (a.sign != b.sign) {
        return a.sign;
    } else if (a.sign) {
        return (-a > -b);
    } else if (a.digits.size() != b.digits.size()) {
        return a.digits.size() < b.digits.size();
    } else {
        for (int i = (int)a.digits.size() - 1; i >= 0; i--)
            if (a.digits[i] != b.digits[i])
                return a.digits[i] < b.digits[i];
        return false;
    }
}

bool operator!=(const big_integer &a, const big_integer &b)
{
    return !(a == b);
}

bool operator==(const big_integer &a, const big_integer &b)
{
    return (a.sign == b.sign && a.digits == b.digits);
}

void big_integer::swap(big_integer &second)
{
    std::swap(digits, second.digits);
    std::swap(sign, second.sign);
}

void big_integer::delete_zeros()
{
    while (digits.size() > 1 && digits.back() == 0)
        digits.pop_back();
}

void big_integer::addition_to_two(size_t length)
{
    assert(length >= digits.size());
    while (digits.size() != length)
        digits.push_back(0);
    if (sign) {
        sign = false;
        for (size_t i = 0; i < digits.size(); i++)
            digits[i] = ~digits[i];
        *this += 1;
    }
}

big_integer abstract_bitwise_operation(big_integer a, const big_integer &b, std::function<uint32_t (uint32_t, uint32_t)> how)
{
    big_integer first = a, second = b;
    big_integer result;
    size_t sz = std::max(a.digits.size(), b.digits.size());
    first.addition_to_two(sz);
    second.addition_to_two(sz);
    result.addition_to_two(sz);
    for (size_t i = 0; i < sz; i++)
        result.digits[i] = how(first.digits[i], second.digits[i]);
    if (how((uint32_t)a.sign, (uint32_t)b.sign)) {
        result = -result;
        result.addition_to_two(sz);
        result = -result;
    }
    return result;
}

big_integer operator+(big_integer a, const big_integer &b)
{
    if (a.sign != b.sign)
        return (a.sign ? b - (-a) : a - (-b));
    size_t sz = std::max(a.digits.size(), b.digits.size());
    big_integer result;
    result.digits.resize(sz);
    bool carry = false;
    uint64_t sum;
    for (size_t  i = 0; i < sz; i++) {
        sum = (carry ? 1 : 0);
        if (i < a.digits.size())
            sum += a.digits[i];
        if (i < b.digits.size())
            sum += b.digits[i];
        carry = (sum > UINT32_MAX);
        result.digits[i] = (uint32_t)(sum & UINT32_MAX);
    }
    if (carry)
        result.digits.push_back(1);
    result.sign = a.sign;
    result.delete_zeros();
    return result;
}

big_integer operator%(big_integer a, const big_integer &b)
{
    return a - (a / b) * b;
}

std::function<uint32_t(uint32_t, uint32_t)> _and = [](uint32_t a, uint32_t b) {return a & b;};
std::function<uint32_t(uint32_t, uint32_t)> _or = [](uint32_t a, uint32_t b) {return a | b;};
std::function<uint32_t(uint32_t, uint32_t)> _xor = [](uint32_t a, uint32_t b) {return a ^ b;};

big_integer operator&(big_integer a, const big_integer &b)
{
    return abstract_bitwise_operation(a, b, _and);
}

big_integer operator|(big_integer a, const big_integer &b)
{
    return abstract_bitwise_operation(a, b, _or);
}

big_integer operator^(big_integer a, const big_integer &b)
{
    return abstract_bitwise_operation(a, b, _xor);
}

std::ostream &operator<<(std::ostream &s, big_integer const &a)
{
    return s << to_string(a);
}

big_integer operator<<(big_integer a, int shift)
{
    a *= ((uint32_t) 1 << (shift % 32));
    std::reverse(a.digits.begin(), a.digits.end());
    for (int i = 0; i < (shift / 32); i++)
        a.digits.push_back(0);
    std::reverse(a.digits.begin(), a.digits.end());
    return a;
}
