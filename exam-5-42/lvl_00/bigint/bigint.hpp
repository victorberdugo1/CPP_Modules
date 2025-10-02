#pragma once
#include <string>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cstddef>

// Estructura bigint: representa un entero sin signo de precisión arbitraria
// Almacena el número como un string de dígitos para superar el límite de SIZE_MAX
struct bigint {
    std::string _val; // Almacena el número como string (ej: "12345")
    
    // Constructor por defecto: inicializa a "0"
    bigint() : _val("0") {}
    
    // Constructor desde unsigned int: convierte un entero a string
    bigint(unsigned int i) : _val(i ? "" : "0") { 
        while (i) {
            // Extrae cada dígito (i % 10) y lo convierte a char
            // Lo añade al INICIO del string para mantener el orden correcto
            _val = char('0' + i % 10) + _val;
            i /= 10; // Elimina el último dígito procesado
        }
    }
    
    // Constructor desde string: permite crear bigint desde cadenas como "999999999999"
    bigint(const std::string& s) : _val(s.empty() ? "0" : s) {
        normalize(); // Elimina ceros a la izquierda (ej: "00042" → "42")
    }
    
    // Constructor de copia: crea un bigint idéntico a otro
    bigint(const bigint& o) : _val(o._val) {}
    
    // Operador de asignación: permite a = b
    bigint& operator=(const bigint& o) { 
        _val = o._val;
        return *this;
    }
    
    // SUMA: Implementa la suma de dos bigint (requisito del subject)
    bigint operator+(const bigint& o) const {
        std::string a = _val, b = o._val, res = "";
        // Invertimos los strings para sumar de derecha a izquierda (como a mano)
        std::reverse(a.begin(), a.end());
        std::reverse(b.begin(), b.end());
        int carry = 0, i = 0; // carry = "me llevo una"
        
        // Suma dígito a dígito mientras haya dígitos o carry
        while (i < (int)a.size() || i < (int)b.size() || carry) {
            // Suma: dígito de a + dígito de b + carry
            int sum = carry 
                + (i < (int)a.size() ? a[i] - '0' : 0) // Dígito de a o 0
                + (i < (int)b.size() ? b[i] - '0' : 0); // Dígito de b o 0
            res = char('0' + sum % 10) + res; // Añade el dígito al resultado
            carry = sum / 10; // Calcula el nuevo carry (0 o 1)
            i++;
        }
        return bigint(res);
    }
    
    // Operador +=: permite a += b (suma y asigna)
    bigint& operator+=(const bigint& o) { 
        return *this = *this + o;
    }
    
    // COMPARACIÓN: Igualdad (requisito del subject)
    bool operator==(const bigint& o) const { 
        return _val == o._val; // Compara los strings directamente
    }
    
    // COMPARACIÓN: Desigualdad
    bool operator!=(const bigint& o) const { 
        return !(*this == o);
    }
    
    // COMPARACIÓN: Menor que (requisito del subject)
    bool operator<(const bigint& o) const {
        // Si tienen diferente número de dígitos, el más corto es menor
        if (_val.size() != o._val.size())
            return _val.size() < o._val.size();
        // Si tienen el mismo tamaño, comparación lexicográfica funciona
        return _val < o._val; // "123" < "456" es verdadero
    }
    
    // COMPARACIÓN: Menor o igual
    bool operator<=(const bigint& o) const { 
        return *this < o || *this == o;
    }
    
    // COMPARACIÓN: Mayor que
    bool operator>(const bigint& o) const { 
        return !(*this <= o);
    }
    
    // COMPARACIÓN: Mayor o igual
    bool operator>=(const bigint& o) const { 
        return !(*this < o);
    }
    
    // DIGITSHIFT IZQUIERDA: desplaza dígitos hacia la izquierda (requisito del subject)
    // Ejemplo: 42 << 3 = 42000 (añade 3 ceros al final = multiplica por 10^3)
    bigint operator<<(unsigned int n) const {
        if (_val == "0" || n == 0) return *this; // Casos especiales
        bigint res(*this);
        res._val.append(n, '0'); // Añade n ceros al final del string
        return res;
    }
    
    // DIGITSHIFT DERECHA: desplaza dígitos hacia la derecha (requisito del subject)
    // Ejemplo: 1337 >> 2 = 13 (elimina 2 dígitos del final = divide por 10^2)
    bigint operator>>(unsigned int n) const {
        if (_val == "0" || n == 0) return *this; // Casos especiales
        bigint res(*this);
        // Si desplazamos más dígitos de los que tiene, resultado es 0
        if (n >= res._val.size()) {
            res._val = "0";
            return res;
        }
        // Elimina n caracteres del final del string
        res._val.erase(res._val.size() - n);
        res.normalize(); // Elimina ceros a la izquierda si quedan
        return res;
    }
    
    // Sobrecarga de << con bigint como parámetro
    bigint operator<<(const bigint& o) const { 
        return *this << o.toUInt(); // Convierte el bigint a uint y llama a 
    }
    
    // Sobrecarga de >> con bigint como parámetro
    bigint operator>>(const bigint& o) const { 
        return *this >> o.toUInt(); // Convierte el bigint a uint y llama a >>
    }
    
    // Operador <<=: desplaza y asigna (a <<= 3 es como a = a << 3)
    bigint& operator<<=(unsigned int n) { 
        return *this = *this << n;
    }
    
    // Operador >>=: desplaza y asigna (a >>= 2 es como a = a >> 2)
    bigint& operator>>=(unsigned int n) { 
        return *this = *this >> n;
    }
    
    // Operador <<= con bigint
    bigint& operator<<=(const bigint& o) { 
        return *this = *this << o;
    }
    
    // Operador >>= con bigint
    bigint& operator>>=(const bigint& o) { 
        return *this = *this >> o;
    }
    
    // Post-incremento: a++ (devuelve el valor anterior y luego incrementa)
    bigint operator++(int) { 
        bigint tmp(*this); // Guarda el valor actual
        *this += bigint(1); // Incrementa
        return tmp; // Devuelve el valor anterior
    }
    
    // Pre-incremento: ++a (incrementa y devuelve el nuevo valor)
    bigint& operator++() { 
        return *this = *this + bigint(1);
    }
    
    // Convierte el bigint a unsigned int (útil para operadores de shift)
    unsigned int toUInt() const {
        unsigned long long result = 0;
        // Convierte cada dígito del string a número
        for (size_t i = 0; i < _val.size(); ++i) {
            result = result * 10 + (_val[i] - '0');
            // Si excede el límite de unsigned int, devuelve el máximo
            if (result > static_cast<unsigned long long>(std::numeric_limits<unsigned int>::max())) {
                return std::numeric_limits<unsigned int>::max();
            }
        }
        return static_cast<unsigned int>(result);
    }
    
    // Imprime el bigint por consola
    void print() const { 
        std::cout << _val << std::endl;
    }

private:
    // Normaliza el número eliminando ceros a la izquierda (requisito del subject)
    // Ejemplo: "00042" → "42", "000" → "0"
    void normalize() {
        size_t i = 0;
        // Avanza mientras haya ceros al inicio (pero deja al menos un dígito)
        while (i + 1 < _val.size() && _val[i] == '0') ++i;
        if (i) _val.erase(0, i); // Elimina los ceros encontrados
        if (_val.empty()) _val = "0"; // Si quedó vacío, pone "0"
    }
};

// OPERADOR DE SALIDA: permite imprimir bigint con cout (requisito del subject)
// Ejemplo: cout << bigint(42) << endl; imprime "42"
// IMPORTANTE: la salida NO debe contener ceros a la izquierda (gracias a normalize)
inline std::ostream& operator<<(std::ostream& os, const bigint& b) { 
    return os << b._val; // Imprime el string interno en base 10
}
