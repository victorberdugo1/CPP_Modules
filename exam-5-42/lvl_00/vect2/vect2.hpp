#ifndef VECT2_HPP
#define VECT2_HPP

#include <iostream>

// vect2: vector 2D entero (lectura cómoda en móvil)
class vect2 {
private:
	int x;
	int y;

public:
	// constructores / asignador / destructor
	vect2() : x(0), y(0) {}
	vect2(int nx, int ny) : x(nx), y(ny) {}
	vect2(const vect2& s) : x(s.x), y(s.y) {}
	vect2& operator=(const vect2& s) {
		if (this != &s) {
			x = s.x;
			y = s.y;
		}
		return *this;
	}
	~vect2() {}

	// acceso por índice: 0->x, 1->y (sin comprobación)
	int operator[](int i) const { return (i == 0) ? x : y; }
	int& operator[](int i)       { return (i == 0) ? x : y; }

	// unario -
	vect2 operator-() const { return vect2(-x, -y); }

	// escalar (miembro)
	vect2 operator*(int n) const { return vect2(x * n, y * n); }
	vect2 operator+(int n) const { return vect2(x + n, y + n); }
	vect2 operator-(int n) const { return vect2(x - n, y - n); }

	vect2& operator*=(int n) { x *= n; y *= n; return *this; }
	vect2& operator+=(int n) { x += n; y += n; return *this; }
	vect2& operator-=(int n) { x -= n; y -= n; return *this; }

	// component-wise con vect2
	vect2& operator+=(const vect2& o) { x += o.x; y += o.y; return *this; }
	vect2& operator-=(const vect2& o) { x -= o.x; y -= o.y; return *this; }
	vect2& operator*=(const vect2& o) { x *= o.x; y *= o.y; return *this; }

	vect2 operator+(const vect2& o) const { return vect2(x + o.x, y + o.y); }
	vect2 operator-(const vect2& o) const { return vect2(x - o.x, y - o.y); }
	vect2 operator*(const vect2& o) const { return vect2(x * o.x, y * o.y); }

	// inc / dec
	vect2& operator++()    { ++x; ++y; return *this; }
	vect2  operator++(int) { vect2 tmp(*this); ++(*this); return tmp; }
	vect2& operator--()    { --x; --y; return *this; }
	vect2  operator--(int) { vect2 tmp(*this); --(*this); return tmp; }

	// comparaciones
	bool operator==(const vect2& o) const { return x == o.x && y == o.y; }
	bool operator!=(const vect2& o) const { return !(*this == o); }
};

// operadores libres: escalar a la izquierda
inline vect2 operator*(int n, const vect2& v) { return v * n; }
inline vect2 operator+(int n, const vect2& v) {
	return vect2(v[0] + n, v[1] + n);
}

// salida
inline std::ostream& operator<<(std::ostream& os, const vect2& v) {
	os << "{" << v[0] << ", " << v[1] << "}";
	return os;
}

#endif // VECT2_HPP
