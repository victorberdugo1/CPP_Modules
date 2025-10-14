#ifndef BIGINT_HPP
#define BIGINT_HPP
#include <iostream>
#include <string>
#include <algorithm>

class bigint {
public:
	std::string _val;
	bigint() : _val("0") {}
	bigint(unsigned long long n) : _val(std::to_string(n)) {}
	bigint(const std::string& s) : _val(s.empty() ? "0" : s) {
		normalize();
	}
	void normalize() {
		size_t pos = _val.find_first_not_of('0');
		_val = (pos == std::string::npos) ? "0" : _val.substr(pos);
	}
	unsigned int toUInt() const {
		unsigned long long acc = 0;
		for (char c : _val) {
			if ((acc = acc * 10 + (c - '0')) > UINT_MAX) {
				return UINT_MAX;
			}
		}
		return acc;
	}
	bigint operator+(const bigint& rhs) const {
		std::string res;
		int i = _val.size() - 1, j = rhs._val.size() - 1, carry = 0;
		while (i >= 0 || j >= 0 || carry) {
			int sum = carry;
			if (i >= 0)
				sum += _val[i--] - '0';
			if (j >= 0)
				sum += rhs._val[j--] - '0';
			res += '0' + (sum % 10);
			carry = sum / 10;
		}
		std::reverse(res.begin(), res.end());
		return bigint(res);
	}
	bool operator<(const bigint& o) const {
		if (_val.size() != o._val.size())
			return _val.size() < o._val.size();
		return _val < o._val;
	}
	bigint operator<<(int shift) const {
		if (shift <= 0) return *this;
		return bigint(_val + std::string(shift, '0'));
	}
	bigint operator>>(unsigned int shift) const {
		if (shift == 0 || _val == "0") return *this;
		if (shift >= _val.size()) return bigint(0);
		return bigint(_val.substr(0, _val.size() - shift));
	}
	friend std::ostream& operator<<(std::ostream& os, const bigint& n) {
		return os << n._val;
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// COMPARISONS: Define < and ==, the rest are negations/inversions
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

	// Base: compares the string values
	bool operator==(const bigint& o) const {
		return _val == o._val;
	}

	// != is simply the negation of ==
	bool operator!=(const bigint& o) const {
		return !(*this == o);
	}

	// > swaps the operands: a > b is the same as b < a
	bool operator>(const bigint& o) const {
		return o < *this;
	}

	// <= is the negation of >: if it's not greater, it's less or equal
	bool operator<=(const bigint& o) const {
		return !(*this > o);
	}

	// >= is the negation of <: if it's not less, it's greater or equal
	bool operator>=(const bigint& o) const {
		return !(*this < o);
	}


	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// Pattern *this = *this [operator] value
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

	// a += b  →  a = a + b
	bigint& operator+=(const bigint& rhs) {
		return *this = *this + rhs;
	}

	// a <<= n  →  a = a << n
	bigint& operator<<=(int shift) {
		return *this = *this << shift;
	}

	// a >>= n  →  a = a >> n
	bigint& operator>>=(int shift) {
		return *this = *this >> shift;
	}

	// Versions with bigint: convert to uint and use the int version
	bigint& operator<<=(const bigint& shift) {
		return *this = *this << shift;
	}

	bigint& operator>>=(const bigint& shift) {
		return *this = *this >> shift;
	}


	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// INCREMENT: ++a returns the new value, a++ returns the old value
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

	// Pre-increment (++a): increments and returns the new value
	bigint& operator++() {
		return *this += bigint(1);
	}

	// Post-increment (a++): returns the old value, then increments
	// The (int) is fictitious, only to differentiate
	bigint operator++(int) {
		bigint tmp(*this);  // Save current value
		++*this;            // Increment using pre-increment
		return tmp;         // Return the old value
	}


	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// OVERLOADS: Convert bigint to int and reuse existing operators
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

	// Allows using bigint as a shift amount
	bigint operator<<(const bigint& shift) const {
		return *this << shift.toUInt();
	}

	bigint operator>>(const bigint& shift) const {
		return *this >> shift.toUInt();
	}

	bigint operator-(const bigint&) const { return bigint(0); }
};
#endif
