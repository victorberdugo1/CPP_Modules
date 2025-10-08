#pragma once

#include <string>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cstddef>

// Estructura bigint: representa un entero
// sin signo de precisión arbitraria.
// Valor decimal en _val (sin ceros iniciales).
struct bigint {

    std::string _val; // "0", "42", "123..."

    // --- Constructores -----------------------------------------------
    bigint() : _val("0") {}

    bigint(unsigned int i) : _val(i ? "" : "0") {
        while (i) {
            _val = char('0' + (i % 10)) + _val;
            i /= 10;
        }
    }

    bigint(const std::string& s)
        : _val(s.empty() ? "0" : s)
    {
        normalize();
    }

    bigint(const bigint& o) : _val(o._val) {}

    bigint& operator=(const bigint& o) {
        _val = o._val;
        return *this;
    }

    // --- Aritmética --------------------------------------------------
    bigint operator+(const bigint& o) const {
        std::string a = _val;
        std::string b = o._val;
        std::string res = "";

        std::reverse(a.begin(), a.end());
        std::reverse(b.begin(), b.end());

        int carry = 0;
        size_t i = 0;

        while (i < a.size() || i < b.size() || carry) {
            int da = (i < a.size()) ? (a[i] - '0') : 0;
            int db = (i < b.size()) ? (b[i] - '0') : 0;
            int sum = da + db + carry;

            res = char('0' + (sum % 10)) + res;
            carry = sum / 10;
            ++i;
        }

        return bigint(res);
    }

    bigint& operator+=(const bigint& o) {
        return *this = *this + o;
    }

    // --- Comparaciones ----------------------------------------------
    bool operator==(const bigint& o) const { return _val == o._val; }
    bool operator!=(const bigint& o) const { return !(*this == o); }

    bool operator<(const bigint& o) const {
        if (_val.size() != o._val.size())
            return _val.size() < o._val.size();
        return _val < o._val;
    }

    bool operator<=(const bigint& o) const {
        return *this < o || *this == o;
    }

    bool operator>(const bigint& o) const { return !(*this <= o); }
    bool operator>=(const bigint& o) const { return !(*this < o); }

    // --- Shifts (multiplica / divide por 10^n) ----------------------
    bigint operator<<(unsigned int n) const {
        if (_val == "0" || n == 0) return *this;
        bigint res(*this);
        res._val.append(n, '0');
        return res;
    }

    bigint operator>>(unsigned int n) const {
        if (_val == "0" || n == 0) return *this;
        bigint res(*this);
        if (n >= res._val.size()) {
            res._val = "0";
            return res;
        }
        res._val.erase(res._val.size() - n);
        res.normalize();
        return res;
    }

    bigint operator<<(const bigint& o) const {
        return *this << o.toUInt();
    }

    bigint operator>>(const bigint& o) const {
        return *this >> o.toUInt();
    }

    bigint& operator<<=(unsigned int n) { return *this = *this << n; }
    bigint& operator>>=(unsigned int n) { return *this = *this >> n; }
    bigint& operator<<=(const bigint& o) { return *this = *this << o; }
    bigint& operator>>=(const bigint& o) { return *this = *this >> o; }

    // --- Incremento -------------------------------------------------
    bigint operator++(int) {
        bigint tmp(*this);
        *this += bigint(1);
        return tmp;
    }

    bigint& operator++() {
        return *this = *this + bigint(1);
    }

    // --- Utilidades -------------------------------------------------
    unsigned int toUInt() const {
        unsigned long long result = 0;
        for (size_t i = 0; i < _val.size(); ++i) {
            result = result * 10 + (_val[i] - '0');
            if (result > static_cast<unsigned long long>(
                    std::numeric_limits<unsigned int>::max()))
                return std::numeric_limits<unsigned int>::max();
        }
        return static_cast<unsigned int>(result);
    }

    void print() const { std::cout << _val << '
'; }

private:
    // Elimina ceros iniciales (deja "0" si corresponde)
    void normalize() {
        size_t i = 0;
        while (i + 1 < _val.size() && _val[i] == '0') ++i;
        if (i) _val.erase(0, i);
        if (_val.empty()) _val = "0";
    }
};

inline std::ostream& operator<<(std::ostream& os, const bigint& b) {
    return os << b._val;
}
