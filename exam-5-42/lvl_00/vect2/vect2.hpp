#ifndef VECT2_HPP
#define VECT2_HPP
#include <iostream>

// Clase vector 2D con coordenadas enteras
class vect2
{
private:
	int x; // Componente X
	int y; // Componente Y

public:
	/* Constructores */
	
	// Constructor por defecto: (0, 0)
	vect2() : x(0), y(0) {}
	
	// Constructor con parámetros
	vect2(int num1, int num2) 
		: x(num1), y(num2) {}
	
	// Constructor de copia
	vect2(const vect2& source) 
		: x(source.x), y(source.y) {}
	
	// Operador de asignación
	vect2& operator=(const vect2& source) {
		// Evita auto-asignación
		if (this != &source) { 
			x = source.x; 
			y = source.y; 
		}
		return *this;
	}
	
	// Destructor
	~vect2() {}
	
	/* Acceso por índice */
	
	// Lectura: v[0] = x, v[1] = y
	int operator[](int index) const { 
		return (index == 0) ? x : y; 
	}
	
	// Escritura: v[0] = 5
	int& operator[](int index) { 
		return (index == 0) ? x : y; 
	}
	
	/* Operador unario */
	
	// Negación: -v invierte el signo
	vect2 operator-() const { 
		return vect2(-x, -y); 
	}
	
	/* Operaciones con escalar */
	
	// Multiplicación: v * 3
	vect2 operator*(int num) const { 
		return vect2(x * num, y * num); 
	}
	
	// Suma: v + 5
	vect2 operator+(int num) const { 
		return vect2(x + num, y + num); 
	}
	
	// Resta: v - 5
	vect2 operator-(int num) const { 
		return vect2(x - num, y - num); 
	}
	
	/* Operadores compuestos (escalar) */
	
	// Multiplica y asigna: v *= 3
	vect2& operator*=(int num) { 
		x *= num; 
		y *= num; 
		return *this; 
	}
	
	// Suma y asigna: v += 5
	vect2& operator+=(int num) { 
		x += num; 
		y += num; 
		return *this; 
	}
	
	// Resta y asigna: v -= 5
	vect2& operator-=(int num) { 
		x -= num; 
		y -= num; 
		return *this; 
	}
	
	/* Operadores compuestos (vectorial) */
	
	// Suma vectores: v1 += v2
	vect2& operator+=(const vect2& obj) { 
		x += obj.x; 
		y += obj.y; 
		return *this; 
	}
	
	// Resta vectores: v1 -= v2
	vect2& operator-=(const vect2& obj) { 
		x -= obj.x; 
		y -= obj.y; 
		return *this; 
	}
	
	// Multiplica componentes: v1 *= v2
	vect2& operator*=(const vect2& obj) { 
		x *= obj.x; 
		y *= obj.y; 
		return *this; 
	}
	
	/* Operaciones binarias */
	
	// Suma: v1 + v2
	vect2 operator+(const vect2& obj) const { 
		return vect2(x + obj.x, y + obj.y); 
	}
	
	// Resta: v1 - v2
	vect2 operator-(const vect2& obj) const { 
		return vect2(x - obj.x, y - obj.y); 
	}
	
	// Multiplica componentes: v1 * v2
	vect2 operator*(const vect2& obj) const { 
		return vect2(x * obj.x, y * obj.y); 
	}
	
	/* Incrementos y decrementos */
	
	// Pre-incremento: ++v
	vect2& operator++() { 
		++x; 
		++y; 
		return *this; 
	}
	
	// Post-incremento: v++
	vect2 operator++(int) { 
		vect2 tmp(*this); 
		++(*this); 
		return tmp; 
	}
	
	// Pre-decremento: --v
	vect2& operator--() { 
		--x; 
		--y; 
		return *this; 
	}
	
	// Post-decremento: v--
	vect2 operator--(int) { 
		vect2 tmp(*this); 
		--(*this); 
		return tmp; 
	}
	
	/* Comparaciones */
	
	// Igualdad: v1 == v2
	bool operator==(const vect2& obj) const { 
		return x == obj.x && y == obj.y; 
	}
	
	// Desigualdad: v1 != v2
	bool operator!=(const vect2& obj) const { 
		return !(*this == obj); 
	}
};

/* Operadores libres (fuera de clase) */

// Multiplicación conmutativa: 3 * v
inline vect2 operator*(
	int num, 
	const vect2& obj
) {
	return obj * num;
}

// Suma conmutativa: 5 + v
inline vect2 operator+(
	int num, 
	const vect2& obj
) {
	return vect2(
		obj[0] + num, 
		obj[1] + num
	);
}

// Operador de salida: cout << v
inline std::ostream& operator<<(
	std::ostream& os, 
	const vect2& obj
) {
	os << "{" 
	   << obj[0] 
	   << ", " 
	   << obj[1] 
	   << "}";
	return os;
}

#endif // VECT2_HPP
