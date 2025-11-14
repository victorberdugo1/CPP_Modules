#ifndef VECT2_HPP
#define VECT2_HPP

#include <iostream>

class vect2 {
public:
    int x , y;

    // Constructores
    vect2() : x(0),y(0) {}
    vect2(int x, int y) : x(x), y(y) {}

    // Acceso por índice
    int& operator[](int index) const {
        return const_cast<int&>(index == 0 ? this->x : this->y);
    }
    friend std::ostream& operator<<(std::ostream& os,const vect2& v){
        return os << "{" << v[0] << ", " << v[1] << "}";
    }

    // operaciones aritméticas principales
    vect2 operator+(const vect2& o) const {
        return vect2(x + o.x, y + o.y);
    }
    vect2 operator-(const vect2& o) const {
        return vect2(x - o.x, y - o.y);
    }
    vect2 operator*(int n) const {
        return vect2(x * n, y * n);
    }

    // multiplicación con entero por la izquierda (no miembro amigo)
    friend vect2 operator*(int n, const vect2& v) {
        return v * n;
    }

    // operador unario
    vect2 operator-() const {
        return vect2(-x, -y);
    }

    // asignaciones compuestas
    vect2& operator+=(const vect2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    vect2& operator-=(const vect2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    vect2& operator*=(int n) {
        x *= n;
        y *= n;
        return *this;
    }

    // prefijo / sufijo ++ y --
    vect2& operator++() {        // prefijo
        ++x; ++y;
        return *this;
    }
    vect2 operator++(int) {     // sufijo
        vect2 temp(*this);
        ++*this;
        return temp;
    }

    vect2& operator--() {       // prefijo
        --x; --y;
        return *this;
    }
    vect2 operator--(int) {     // sufijo
        vect2 temp(*this);
        --*this;
        return temp;
    }

    // comparaciones (al final)
    bool operator==(const vect2& v) const {
        return x == v.x && y == v.y;
    }
    bool operator!=(const vect2& v) const {
        return !(*this == v);
    }
};

#endif
