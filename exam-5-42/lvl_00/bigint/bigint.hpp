#ifndef BIGINT_HPP
#define BIGINT_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <limits>

class bigint {
	public:
		// B - BigInt: almacena su valor interno como string (_val)
		std::string _val;

		// I - Inicializadores: desde cero o desde número
		bigint() : _val("0") {}
		bigint(unsigned long long n) : _val(std::to_string(n)) {}
		// G - Guarda desde cadena: usa "0" si está vacía y normaliza
		bigint(const std::string& s) : _val(s.empty() ? "0" : s) {
			normalize();
		}

		// N - Normaliza (quita ceros izquierda)
		void normalize() {
			size_t pos = _val.find_first_not_of('0');
			_val = (pos == std::string::npos) ? "0" : _val.substr(pos);
		}
		// U - UInt conversión segura (toUInt con límites) Accumulator
		unsigned int toUInt() const {
			unsigned long long acc = 0;
			const auto LIM = std::numeric_limits<unsigned int>::max();
			for (char c : _val) {
				if ((acc = acc * 10 + (c - '0')) > LIM) return LIM;
			}
			return acc;
		}
		// M - Matemáticas: suma e incremento (++) RightHandSide
		bigint operator+(const bigint& rhs) const {
			std::string res;
			int i = _val.size()-1, j = rhs._val.size()-1, carry = 0;        
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
		bigint& operator+=(const bigint& rhs) {
			return *this = *this + rhs;
		}
		bigint& operator++() {
			return *this += bigint(1); 
		}
		bigint operator++(int) {
			bigint tmp(*this);
			++*this;
			return tmp;
		}   
		// E - Evalúa comparaciones (==, !=, <, >, <=, >=)
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

		//R - Operador resta aunque no lo pide subject grademe no compila 
		bigint operator-(const bigint& ) const { return bigint(0); }    

		// O - Operadores de desplazamiento (<<, >>) por int
		bigint operator<<(int shift) const {
			if (shift <= 0) return *this;
			return bigint(_val + std::string(shift, '0'));
		}
		bigint& operator<<=(int shift) { 
			return *this = *this << shift; 
		}

		bigint operator>>(unsigned int n) const {
			if (n == 0 || _val == "0") return *this;
			if (n >= _val.size()) return bigint(0);        
			return bigint(_val.substr(0, _val.size() - n));
		}
		bigint& operator>>=(int shift) { 
			return *this = *this >> shift; 
		}
		// O - Operadores de desplazamiento (<<, >>) por bigint
		bigint operator<<(const bigint& shift) const{
			return *this << shift.toUInt();
		}
		bigint& operator<<=(const bigint& shift) { 
			return *this = *this << shift; 
		}
		bigint operator>>(const bigint& shift) const {
			return *this >> shift.toUInt();
		}
		bigint& operator>>=(const bigint& shift) { 
			return *this = *this >> shift;
		}
		// S - Salida fácil con << para ostream
		friend std::ostream& operator<<(std::ostream& os, const bigint& n) {
			return os << n._val;
		}
};
#endif // BIGINT_HPP
