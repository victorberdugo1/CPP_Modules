#ifndef VECT2_HPP
#define VECT2_HPP
#include <iostream>

// V - Vector 2D con enteros
class vect2 {
	private:
		int x, y;

	public:
		/* E - Establece constructores (default y parámetros) */
		vect2() : x(0), y(0) {}
		vect2(int x, int y) : x(x), y(y) {}

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
			x += o.x; 
			y += o.y;
			return *this;
		}
		vect2& operator-=(const vect2& o) {
			x -= o.x; 
			y -= o.y;
			return *this;
		}
		vect2& operator*=(int n) {
			x *= n; 
			y *= n;
			return *this;
		}

		/* 2 - Multiplicación por escalar * y Comparaciones (== !=) */
		vect2 operator*(int n) const {
			return vect2(x * n, y * n);
		}
		friend inline vect2 operator*(int n, const vect2& v) {
			return v * n;
		}

		bool operator==(const vect2& o) const {
			return x == o.x && y == o.y;
		}
		bool operator!=(const vect2& o) const {
			return x != o.x || y != o.y;
		}

		/* D - Decremento e Incremento (pre/post) */
		vect2& operator++() {
			++x; ++y;
			return *this;
		}
		vect2 operator++(int) {
			vect2 tmp(*this);
			++*this;
			return tmp;
		}
		vect2& operator--() {
			--x; --y;
			return *this;
		}
		vect2 operator--(int) {
			vect2 tmp(*this);
			--*this;
			return tmp;
		}
};

inline std::ostream& operator<<(std::ostream& os, const vect2& v) {
	return os << "{" << v[0] << ", " << v[1] << "}";
}

#endif // VECT2_HPP
