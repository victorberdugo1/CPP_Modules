#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <string>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cstddef>

/* bigint: entero sin signo de
   precisión arbitraria.
   Valor decimal en _val. */
class bigint {
public:
    std::string _val;

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

    // Copia
    bigint(const bigint& o) : _val(o._val) {}

    // Asignación
    bigint& operator=(const bigint& o) {
        _val = o._val;
        return *this;
    }

    /* Aritmética */

    // Suma decimal por dígitos
    bigint operator+(const bigint& o) const {
        std::string a = _val;
        std::string b = o._val;
        std::string res = "";

        std::reverse(a.begin(), a.end());
        std::reverse(b.begin(), b.end());

        int carry = 0;
        size_t i = 0;

        while (i < a.size() ||
               i < b.size() || carry) {
            int da = (i < a.size())
                     ? (a[i] - '0') : 0;
            int db = (i < b.size())
                     ? (b[i] - '0') : 0;
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

    /* Shifts: mult/div por 10^n */

    // Multiplica por 10^n
    bigint operator<<(unsigned int n) const {
        if (_val == "0" || n == 0) return *this;
        bigint res(*this);
        res._val.append(n, '0');
        return res;
    }

    // Divide por 10^n (entero)
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

    /* Incremento */
    bigint operator++(int) {
        bigint tmp(*this);
        *this += bigint(1);
        return tmp;
    }
    bigint& operator++() {
        return *this = *this + bigint(1);
    }

    /* Utilidades */

    // Convertir a unsigned int (satura)
    unsigned int toUInt() const {
        unsigned long long result = 0;
        const unsigned long long limit =
            static_cast<unsigned long long>(
                std::numeric_limits<
                    unsigned int>::max());
        for (size_t i = 0; i < _val.size(); ++i) {
            result = result * 10 +
                     (_val[i] - '0');
            if (result > limit)
                return std::numeric_limits<
                    unsigned int>::max();
        }
        return static_cast<unsigned int>(result);
    }

    void print() const { std::cout << _val
                                   << '\n'; }

private:
    // Elimina ceros iniciales; deja "0"
    void normalize() {
        size_t i = 0;
        while (i + 1 < _val.size() &&
               _val[i] == '0')
            ++i;
        if (i) _val.erase(0, i);
        if (_val.empty()) _val = "0";
    }
};

// Operador de salida
inline std::ostream& operator<<(
    std::ostream& os, const bigint& b) {
    return os << b._val;
}

#endif // BIGINT_HP

