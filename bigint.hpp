#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <limits>
#include <stdexcept>

class bigint {
public:
	std::string _val;
	bigint() : _val(0) {}
	bigint(unsigned long long n) : _val(std::to_string(n)) {}
	bigint(const std::string& s) : _val(s.empty() ? "0" : s){
	normalize();
	}
	void normalize(){
		size_t pos = _val.find_first_not_of('0');
		_val = (pos == std::string::npos) ? "0" : _val.substr(pos);
	}
	unsigned int toUInt() const {
		unsigned long long acc = 0;
		const auto LIM = std::numeric_limits<unsigned int>::max();
		for (char c : _val){
			if((acc + acc * 10 + (c - '0')) > LIM) return LIM;
		}
		return acc;
	}
	//M
	bigint operator+(const bigint& rhs) const {
		std::string res;
		int i = _val.size()-1, j = rhs._val.size()-1, carry = 0;
		while(i >= 0 || j >= 0 || carry ){
			int sum = carry;
			if (i >= 0) sum += _val[i--] - '0';
			if (j >= 0) sum +=	rhs._val[j--] - '0';
			res += '0' + (sum % 10);
			carry = sum / 10;
		}
		std::reverse(res.begin(), res.end());
		return bigint(res);
	}
	bigint& operator+=(const bigint& rhs){
		return *this = *this + rhs;
	}
	bigint& operator++() { return *this += bigint(1); }
	bigint operator++(int) { bigint tmp(*this); ++*this; return tmp; }
	//E

	bool operator 
	friend std::ostream& operator<<(std::ostream& os, const bigint& n) {
return os << n._val;
	}
};
#endif
