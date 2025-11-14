#ifndef BIGINT_SIMPLE_HPP
#define BIGINT_SIMPLE_HPP

#include <string>
#include <iostream>
#include <algorithm>

class bigint {
private:
	std::string d;
	
	void normalize() {
	    size_t p = d.find_first_not_of('0');
            if (p == std::string::npos)
                d = "0";
            else if (p > 0)
                d = d.substr(p);
	}

public:
	// CONSTRUCTORES
	bigint() : d("0") {}
	bigint(unsigned long long n) : d(std::to_string(n)) {}
	bigint(const std::string& s) : d(s) { normalize(); }

	// SALIDA
	friend std::ostream& operator<<(std::ostream& os, const bigint& b) {
		return os << b.d;
	}
	// SHIFTS
	bigint operator<<(size_t n) const {
		if (d == "0" || n == 0) return *this;
		return bigint(d + std::string(n, '0'));
	}
	
	bigint& operator<<=(size_t n) { 
		*this = *this << n; 
		return *this; 
	}
	
	bigint operator>>(size_t n) const {
		if (n == 0) return *this;
		if (n >= d.size()) return bigint(0);
		return bigint(d.substr(0, d.size() - n));
	}
	
	bigint& operator>>=(size_t n) { 
		*this = *this >> n; 
		return *this; 
	}

	bigint operator<<(const bigint& o) const { 
		size_t n = 0, i = 0;
		while (i < o.d.size()) {
    		     n = n * 10 + (o.d[i] - '0');
    		    i++;
		}
		return *this << n;
	}
	
	bigint& operator<<=(const bigint& o) { 
		*this = *this << o; 
		return *this; 
	}
	
	bigint operator>>(const bigint& o) const { 
		size_t n = 0, i = 0;
		while (i < o.d.size()) {
    		     n = n * 10 + (o.d[i] - '0');
    		    i++;
		}
		return *this >> n;
	}
	
	bigint& operator>>=(const bigint& o) { 
		*this = *this >> o; 
		return *this; 
	}
	// SUMA
	bigint operator+(const bigint& rhs) const {
		std::string res;
		
		int i = d.size() - 1, j = rhs.d.size() - 1, carry = 0;
		
		while (i >= 0 || j >= 0 || carry) {
			int sum = carry;
			if (i >= 0) sum += d[i--] - '0';
			if (j >= 0) sum += rhs.d[j--] - '0';
			res += '0' + (sum % 10);
			carry = sum / 10;
		}
		std::reverse(res.begin(), res.end());
		return bigint(res);
	}
	
	bigint& operator+=(const bigint& rhs) {
		*this = *this + rhs;
		return *this;
	}

	bigint& operator++() {
		*this += bigint(1); 
		return *this; 
	}
	
	bigint operator++(int) {
		bigint tmp = *this; 
		++*this; 
		return tmp; 
	}
	// COMPARACIONES
	bool operator==(const bigint& o) const { 
		return d == o.d;
	}
	
	bool operator!=(const bigint& o) const { 
		return d != o.d; 
	}
	
	bool operator<(const bigint& o) const {
		if (d.size() != o.d.size()) 
			return d.size() < o.d.size();
		return d < o.d;
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


};

#endif