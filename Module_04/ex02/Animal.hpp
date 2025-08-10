
#pragma once

#include <iostream>
#include "Brain.hpp"

class Animal
{
	protected:
		Animal(void);
		Animal(std::string const &type);
		Animal(Animal const &copy);
		std::string	_type;
	public:
		/* Constructors & Destructors */
		virtual ~Animal(void);

		/* Basic Operators */
		Animal const	&operator=(Animal const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		void				setType(std::string const &type);
		virtual Brain 		&getBrain(void) const = 0;

		/* Main member functions */
		virtual void	makeSound(void) const;
};
