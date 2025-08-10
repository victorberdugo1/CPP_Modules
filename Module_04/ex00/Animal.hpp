
#pragma once

#include <iostream>

class Animal
{
	protected:
		std::string	_type;
	public:
		/* Constructors & Destructors */
		Animal(void);
		Animal(std::string const &type);
		Animal(Animal const &copy);
		virtual ~Animal(void);

		/* Basic Operators */
		Animal const	&operator=(Animal const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		void				setType(std::string const &type);

		/* Main member functions */
		virtual void	makeSound(void) const;
};
