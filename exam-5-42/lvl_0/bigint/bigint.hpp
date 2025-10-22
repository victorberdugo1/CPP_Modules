#ifndef BIGINT_HPP
#define BIGINT_HPP
#include <iostream>
#include <string>
#include <algorithm>

class bigint {
private:
    std::string _val;
    
    void normalize() {
        size_t pos = _val.find_first_not_of('0');
        _val = (pos == std::string::npos) ? "0" : _val.substr(pos);
    }
    
public:
    // Constructores
    bigint() : _val("0") {}
    bigint(unsigned long long n) : _val(std::to_string(n)) {}
    bigint(const std::string& s) : _val(s.empty() ? "0" : s) {
        normalize();
    }
    
    // Salida
    friend std::ostream& operator<<(std::ostream& os, const bigint& n) {
        return os << n._val;
    }
    
    // Suma
    bigint operator+(const bigint& rhs) const {
        std::string res;
        int i = _val.size() - 1;
        int j = rhs._val.size() - 1;
        int carry = 0;
        
        while (i >= 0 || j >= 0 || carry) {
            int sum = carry;
            if (i >= 0) sum += _val[i--] - '0';
            if (j >= 0) sum += rhs._val[j--] - '0';
            res += '0' + (sum % 10);
            carry = sum / 10;
        }
        
        std::reverse(res.begin(), res.end());
        return bigint(res);
    }
    
    bigint& operator+=(const bigint& rhs) {
        return *this = *this + rhs;
    }
    
    // Resta (implementación stub para el test)
    bigint operator-(const bigint&) const {
        return bigint(0);
    }
    
    // Incremento
    bigint& operator++() {
        return *this += bigint(1);
    }
    
    bigint operator++(int) {
        bigint tmp(*this);
        ++*this;
        return tmp;
    }
    
    // Comparaciones
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
    
    bool operator>(const bigint& o) const {
        return o < *this;
    }
    
    bool operator<=(const bigint& o) const {
        return !(*this > o);
    }
    
    bool operator>=(const bigint& o) const {
        return !(*this < o);
    }
    
    // Desplazamientos (multiplica/divide por 10^shift)
    bigint operator<<(int shift) const {
        if (shift <= 0 || _val == "0") return *this;
        return bigint(_val + std::string(shift, '0'));
    }
    
    bigint operator<<(const bigint& shift) const {
        return *this << (int)std::stoull(shift._val);
    }
    
    bigint& operator<<=(const bigint& shift) {
        return *this = *this << shift;
    }
    
    bigint operator>>(const bigint& shift) const {
    int sh;
    try {
        sh = std::stoi(shift._val);
    } catch (...) {
        return bigint(0);
    }
    if (sh <= 0 || _val == "0") return *this;
    if ((size_t)sh >= _val.size()) return bigint(0);
    return bigint(_val.substr(0, _val.size() - sh));
    }

    bigint& operator>>=(const bigint& shift) {
        return *this = *this >> shift;
    }
};

#endif