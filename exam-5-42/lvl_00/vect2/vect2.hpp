#ifndef VECT2_HPP
#define VECT2_HPP

#include <iostream>

class vect2
{
private:
	int x;
	int y;

public:
	/* constructores / asignador / destructor */
	vect2() : x(0), y(0) {}
	vect2(int num1, int num2) : x(num1), y(num2) {}
	vect2(const vect2& source) : x(source.x), y(source.y) {}
	vect2& operator=(const vect2& source) {
		if (this != &source) { x = source.x; y = source.y; }
		return *this;
	}
	~vect2() {}

	/* acceso por índice (0 -> x, 1 -> y). Sin comprobación de índices. */
	int operator[](int index) const { return (index == 0) ? x : y; }
	int& operator[](int index)       { return (index == 0) ? x : y; }

	/* unario - */
	vect2 operator-() const { return vect2(-x, -y); }

	/* multiplicación por escalar (miembro) */
	vect2 operator*(int num) const { return vect2(x * num, y * num); }

	/* suma/resta por escalar (miembro) */
	vect2 operator+(int num) const { return vect2(x + num, y + num); }
	vect2 operator-(int num) const { return vect2(x - num, y - num); }

	/* operadores compuestos con escalar */
	vect2& operator*=(int num) { x *= num; y *= num; return *this; }
	vect2& operator+=(int num) { x += num; y += num; return *this; }
	vect2& operator-=(int num) { x -= num; y -= num; return *this; }

	/* operadores compuestos con vect2 (component-wise) */
	vect2& operator+=(const vect2& obj) { x += obj.x; y += obj.y; return *this; }
	vect2& operator-=(const vect2& obj) { x -= obj.x; y -= obj.y; return *this; }
	vect2& operator*=(const vect2& obj) { x *= obj.x; y *= obj.y; return *this; }

	/* operaciones binarias (component-wise) */
	vect2 operator+(const vect2& obj) const { return vect2(x + obj.x, y + obj.y); }
	vect2 operator-(const vect2& obj) const { return vect2(x - obj.x, y - obj.y); }
	vect2 operator*(const vect2& obj) const { return vect2(x * obj.x, y * obj.y); }

	/* incrementos y decrementos */
	vect2& operator++()    { ++x; ++y; return *this; }      // pre
	vect2  operator++(int) { vect2 tmp(*this); ++(*this); return tmp; } // post
	vect2& operator--()    { --x; --y; return *this; }      // pre
	vect2  operator--(int) { vect2 tmp(*this); --(*this); return tmp; } // post

	/* comparaciones */
	bool operator==(const vect2& obj) const { return x == obj.x && y == obj.y; }
	bool operator!=(const vect2& obj) const { return !(*this == obj); }
};

/* operadores libres: escalar a la izquierda (necesarios para 2 * v y 2 + v) */
inline vect2 operator*(int num, const vect2& obj) {
	return obj * num; // utiliza el miembro
}
inline vect2 operator+(int num, const vect2& obj) {
	return vect2(obj[0] + num, obj[1] + num);
}

/* operador de salida */
inline std::ostream& operator<<(std::ostream& os, const vect2& obj) {
	os << "{" << obj[0] << ", " << obj[1] << "}";
	return os;
}

#endif // VECT2_HPP

