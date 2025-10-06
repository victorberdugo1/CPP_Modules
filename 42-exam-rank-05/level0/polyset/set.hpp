#include "searchable_bag.hpp"

class set
{
private:
    searchable_bag* bag;  // Cambiado de referencia a puntero
    
public:
    set();  // Constructor por defecto
    set(searchable_bag& s_bag);  // Constructor con parámetro
    set(const set& source);  // Constructor de copia
    set& operator=(const set& source);  // Operador de asignación
    ~set();  // Destructor

    bool has(int) const;
    void insert(int);
    void insert(int*, int);
    void print() const;
    void clear();

    const searchable_bag& get_bag() const;
};
