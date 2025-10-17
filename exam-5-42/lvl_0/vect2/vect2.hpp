#ifndef VECT2_HPP
#define VECT2_HPP
#include <iostream>

// Vector 2D con enteros
class vect2 {
private:
	int x, y;

public:
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// CONSTRUCTORES
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	vect2() : x(0), y(0) {}
	vect2(int x, int y) : x(x), y(y) {}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// ACCESO A COMPONENTES: [] (const y no-const)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	int operator[](int index) const {
		return (index == 0) ? x : y;
	}
	int& operator[](int index) {
		return (index == 0) ? x : y;
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// OPERACIONES VECTORIALES: + - (binarios)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	vect2 operator+(const vect2& o) const {
		return vect2(x + o.x, y + o.y);
	}
	vect2 operator-(const vect2& o) const {
		return vect2(x - o.x, y - o.y);
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// OPERADOR UNARIO: negativo (-)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	vect2 operator-() const {
		return vect2(-x, -y);
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// MULTIPLICACIÓN POR ESCALAR: * (ambas direcciones)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	vect2 operator*(int n) const {
		return vect2(x * n, y * n);
	}
	friend inline vect2 operator*(int n, const vect2& v) {
		return v * n;
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// COMPARACIONES: == y !=
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	bool operator==(const vect2& o) const {
		return x == o.x && y == o.y;
	}
	bool operator!=(const vect2& o) const {
		return x != o.x || y != o.y;
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// OPERADORES COMPUESTOS: += -= *=
	// Patrón: *this = *this [operador] valor
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
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

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// INCREMENTO/DECREMENTO: ++/-- (pre y post)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// Pre-incremento (++v): incrementa y retorna el nuevo valor
	vect2& operator++() {
		++x; ++y;
		return *this;
	}
	// Post-incremento (v++): retorna el viejo valor, luego incrementa
	vect2 operator++(int) {
		vect2 tmp(*this);
		++*this;
		return tmp;
	}
	// Pre-decremento (--v): decrementa y retorna el nuevo valor
	vect2& operator--() {
		--x; --y;
		return *this;
	}
	// Post-decremento (v--): retorna el viejo valor, luego decrementa
	vect2 operator--(int) {
		vect2 tmp(*this);
		--*this;
		return tmp;
	}

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// SALIDA: operador 
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	friend std::ostream& operator<<(std::ostream& os, const vect2& v) {
		return os << "{" << v[0] << ", " << v[1] << "}";
	}
};

#endif // VECT2_HPP
