// Header-protection
#pragma once

// Includes
#include <string>
#include <iostream>
#include <stdint.h> // otherwise uintptr_t won't compile with c++98 standard

// classes

typedef struct Data
{
	std::string	name;
	size_t		age;
	Data		*next;
}				Data;

class Serializer
{
	private:
		// private Members
	public:
	// Constructors
		Serializer();
		Serializer(const Serializer &src);

	// Deconstructors
		~Serializer();

	// Overloaded Operators
		Serializer &operator=(const Serializer &src);

	// Public Methods
		uintptr_t serialize(Data *ptr);
		Data *unserialize(uintptr_t raw);
	// Getter

	// Setter

};

// Ostream overload
// std::ostream	&operator<<(std::ostream &o, Data *a);
