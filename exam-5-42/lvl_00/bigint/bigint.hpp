#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <string>
#include <algorithm>
#include <iostream>
#include <limits>

// bigint: entero sin signo de precisión arbitraria
class bigint {
private:
    std::string _val;

    // Elimina ceros iniciales; deja "0"
    void normalize() {
        size_t i = 0;
        while (i + 1 < _val.size() && _val[i] == '0')
            ++i;
        if (i) _val.erase(0, i);
        if (_val.empty()) _val = "0";
    }

    // Convertir a unsigned int (satura)
    unsigned int toUInt() const {
        unsigned long long result = 0;
        const unsigned long long limit =
            static_cast<unsigned long long>(
                std::numeric_limits<unsigned int>::max());
        for (size_t i = 0; i < _val.size(); ++i) {
            result = result * 10 + (_val[i] - '0');
            if (result > limit)
                return std::numeric_limits<unsigned int>::max();
        }
        return static_cast<unsigned int>(result);
    }

public:
    /* Constructores */

    // Por defecto -> "0"
    bigint() : _val("0") {}

    // Desde unsigned int
    bigint(unsigned int i) : _val(i ? "" : "0") {
        while (i) {
            _val = char('0' + (i % 10)) + _val;
            i /= 10;
        }
    }

    // Desde string (normaliza)
    bigint(const std::string& s)
        : _val(s.empty() ? "0" : s)
    {
        normalize();
    }

    /* Suma */

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

    /* Incremento */

    // ++b (prefijo)
    bigint& operator++() {
        return *this = *this + bigint(1);
    }

    // b++ (postfijo)
    bigint operator++(int) {
        bigint tmp(*this);
        *this += bigint(1);
        return tmp;
    }

    /* Comparaciones */

    bool operator==(const bigint& o) const {
        return _val == o._val;
    }

    bool operator!=(const bigint& o) const {
        return !(*this == o);
    }

    bool operator<(const bigint& o) const {
        if (_val.size() != o._val.size())
            return _val.size() < o._val.size();
        return _val < o._val;
    }

    bool operator<=(const bigint& o) const {
        return *this < o || *this == o;
    }

    bool operator>(const bigint& o) const {
        return !(*this <= o);
    }

    bool operator>=(const bigint& o) const {
        return !(*this < o);
    }

    /* Digitshift: mult/div por 10^n */

    // Multiplica por 10^n (42 << 3 == 42000)
    bigint operator<<(unsigned int n) const {
        if (_val == "0" || n == 0) return *this;
        bigint res(*this);
        res._val.append(n, '0');
        return res;
    }

    // Divide por 10^n (1337 >> 2 == 13)
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

    // Versiones que aceptan bigint
    bigint operator<<(const bigint& o) const {
        return *this << o.toUInt();
    }

    bigint operator>>(const bigint& o) const {
        return *this >> o.toUInt();
    }

    // Versiones compuestas
    bigint& operator<<=(unsigned int n) {
        return *this = *this << n;
    }

    bigint& operator>>=(unsigned int n) {
        return *this = *this >> n;
    }

    bigint& operator<<=(const bigint& o) {
        return *this = *this << o;
    }

    bigint& operator>>=(const bigint& o) {
        return *this = *this >> o;
    }

    // Operador de salida (friend para acceder a _val)
    friend std::ostream& operator<<(std::ostream& os, const bigint& b);
};

// Operador de salida
inline std::ostream& operator<<(std::ostream& os, const bigint& b) {
    return os << b._val;
}

#endif // BIGINT_HPP