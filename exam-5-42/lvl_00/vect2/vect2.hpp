#ifndef VECT2_HPP
#define VECT2_HPP
#include <iostream>

// Vector 2D con enteros
class vect2
{
private:
	int x; // X
	int y; // Y

public:
	/* Constructores */
	
	// Por defecto: (0, 0)
	vect2() : x(0), y(0) {}
	
	// Con parámetros
	vect2(int num1, int num2) 
		: x(num1), y(num2) {}
	
	// Copia
	vect2(const vect2& source) 
		: x(source.x), y(source.y) {}
	
	// Asignación
	vect2& operator=(const vect2& src) {
		if (this != &src) { 
			x = src.x; y = src.y; 
		}
		return *this;
	}
	
	// Destructor
	~vect2() {}
	
	/* Acceso [] */
	
	// Lectura v[0]=x, v[1]=y
	int operator[](int index) const { 
		return (index == 0) ? x : y; 
	}
	
	// Escritura v[0]=5
	int& operator[](int index) { 
		return (index == 0) ? x : y; 
	}
	
	/* Unario */
	
	// Negación -v
	vect2 operator-() const { 
		return vect2(-x, -y); 
	}
	
	/* Con escalar */
	
	// Mult: v * 3
	vect2 operator*(int num) const
	{ 
		return vect2(x*num, y*num); 
	}
	
	// Suma: v + 5
	vect2 operator+(int num) const
	{ 
		return vect2(x+num, y+num); 
	}
	
	// Resta: v - 5
	vect2 operator-(int num) const
	{ 
		return vect2(x-num, y-num); 
	}
	
	/* Compuestos escalar */
	
	// v *= 3
	vect2& operator*=(int num)
	{ 
		x *= num; y *= num; 
		return *this; 
	}
	
	// v += 5
	vect2& operator+=(int num)
	{ 
		x += num; y += num; 
		return *this; 
	}
	
	// v -= 5
	vect2& operator-=(int num)
	{ 
		x -= num; y -= num; 
		return *this; 
	}
	
	/* Compuestos vectorial */
	
	// v1 += v2
	vect2& operator+=(const vect2& o)
	{ 
		x += o.x; y += o.y; 
		return *this; 
	}
	
	// v1 -= v2
	vect2& operator-=(const vect2& o)
	{ 
		x -= o.x; y -= o.y; 
		return *this; 
	}
	
	// v1 *= v2
	vect2& operator*=(const vect2& o)
	{ 
		x *= o.x; y *= o.y; 
		return *this; 
	}
	
	/* Binarias */
	
	// v1 + v2
	vect2 operator+(const vect2& o) const
	{ 
		return vect2(x+o.x, y+o.y); 
	}
	
	// v1 - v2
	vect2 operator-(const vect2& o) const
	{ 
		return vect2(x-o.x, y-o.y); 
	}
	
	// v1 * v2
	vect2 operator*(const vect2& o) const
	{ 
		return vect2(x*o.x, y*o.y); 
	}
	
	/* Inc/Dec */
	
	// ++v
	vect2& operator++() { 
		++x; ++y; return *this; 
	}
	
	// v++
	vect2 operator++(int) { 
		vect2 tmp(*this); ++(*this); 
		return tmp; 
	}
	
	// --v
	vect2& operator--() { 
		--x; --y; return *this; 
	}
	
	// v--
	vect2 operator--(int) { 
		vect2 tmp(*this); --(*this); 
		return tmp; 
	}
	
	/* Comparaciones */
	
	// v1 == v2
	bool operator==(const vect2& o) const
	{ 
		return x==o.x && y==o.y; 
	}
	
	// v1 != v2
	bool operator!=(const vect2& o) const
	{ 
		return !(*this == o); 
	}
};

/* Libres */

// 3 * v
inline vect2 operator*(int n, 
	const vect2& o) {
	return o * n;
}

// 5 + v
inline vect2 operator+(int n, 
	const vect2& o) {
	return vect2(o[0]+n, o[1]+n);
}

// cout << v
inline std::ostream& operator<<(
	std::ostream& os, const vect2& o) {
	os << "{" << o[0] << ", " 
	   << o[1] << "}";
	return os;
}

#endif // VECT2_HPP
