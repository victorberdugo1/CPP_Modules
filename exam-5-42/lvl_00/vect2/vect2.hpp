#ifndef VECT2_HPP
#define VECT2_HPP
#include <iostream>

// V - Vector 2D con enteros
class vect2
{
private:
	int x; // X
	int y; // Y
public:
	/* E - Establece constructores (default y parámetros) */
	vect2() : x(0), y(0) {}
	vect2(int num1, int num2) : x(num1), y(num2) {}
	/* C - Componente acceso con [] (const y no-const) */
	int operator[](int index) const {
		return (index == 0) ? x : y;
	}
	int& operator[](int index) {
		return (index == 0) ? x : y;
	}
	/* T - Trabaja operaciones con vectores (+ -) */
	vect2 operator+(const vect2& o) const {
		return vect2(x + o.x, y + o.y);
	}
	vect2 operator-(const vect2& o) const {
		return vect2(x - o.x, y - o.y);
	}
	/* O - Operador unario negativo */
	vect2 operator-() const {
		return vect2(-x, -y);
	}
	/* R - Realiza operadores compuestos (+= -= *=) */
	vect2& operator+=(const vect2& o) {
		x += o.x; y += o.y;
		return *this;
	}
	vect2& operator-=(const vect2& o) {
		x -= o.x; y -= o.y;
		return *this;
	}
	vect2& operator*=(int num) {
		x *= num; y *= num;
		return *this;
	}

	/* 2 - Multiplicación por escalar * y Comparaciones (== !=) */
	vect2 operator*(int num) const {
		return vect2(x * num, y * num);
	}

	bool operator==(const vect2& o) const {
		return x == o.x && y == o.y;
	}
	bool operator!=(const vect2& o) const {
		return !(*this == o);
	}
	
	/* D - Decremento e Incremento (pre/post) */
	vect2& operator++() {
		++x; ++y;
		return *this;
	}
	vect2 operator++(int) {
		vect2 tmp(*this);
		++(*this);
		return tmp;
	}
	vect2& operator--() {
		--x; --y;
		return *this;
	}
	vect2 operator--(int) {
		vect2 tmp(*this);
		--(*this);
		return tmp;
	}
};

/* Operadores libres CON INLINE para evitar múltiples definiciones */
inline vect2 operator*(int n, const vect2& o) {
	return o * n;
}

inline std::ostream& operator<<(std::ostream& os, const vect2& o) {
	os << "{" << o[0] << ", " << o[1] << "}";
	return os;
}

#endif // VECT2_HPP
