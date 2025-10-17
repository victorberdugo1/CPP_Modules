#ifndef VECT2_HPP
#define VECT2_HPP
#include <iostream>
class vect2 {
private:
	int x;int y;
public:
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// CONSTRUCTORES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	vect2() : x(0),y(0) {}
	vect2(int x, int y) : x(x), y(y) {}
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ACCESO A COMPONENTES: [] (const y no-const)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	int operator[](int index) const {
		return (index == 0) ? x : y ; 
	}
	int& operator[](int index) {
		return (index == 0) ? x : y ; 
	}
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// SALIDA: operador 
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	friend std::ostream& operator<<(std::ostream& os,const vect2& v){
		return os << "{" << v[0] << ", " << v[1] << "}";
	}
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// OPERACIONES
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	bool operator!=(const vect2 v) {
		return x != v.x || y != v.y;
	}
	bool operator==(const vect2 v) {
		return x == v.x && y == v.y;
	}

	vect2 operator-() const {
		return vect2(-x, -y);
	}
	vect2 operator-(const vect2& o) const{
		return vect2(x - o.x, y - o.y);
	}

	vect2 operator+(const vect2& o) const{
		return vect2(x + o.x, y + o.y);
	}

	vect2 operator*(int n) const{
		return vect2(x * n, y * n);
	}
	friend inline vect2 operator*(int n,const vect2& v){
		return v * n;
	}
	vect2& operator*=(int n) {
		x *= n;
		y *= n;
		return *this;
	}

	vect2& operator+=(const vect2 v ) {
		x += v.x;
		y += v.y;
		return *this;
	}
	vect2& operator-=(const vect2 v ) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	vect2& operator--() {
		--x;--y;
		return *this;
	}
	vect2 operator--(int) {
		vect2 temp(*this);
		--*this;
		return temp;
	}
	vect2& operator++() {
		++x;++y;
		return *this;
	}
	vect2 operator++(int) {
		vect2 temp(*this);
		++*this;
		return temp;
	}
};
#endif
