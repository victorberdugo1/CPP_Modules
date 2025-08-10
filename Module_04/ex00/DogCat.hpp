
#pragma once

#include "Animal.hpp"

class Dog: public Animal
{
	private:
		std::string	_type;
	public:
		/* Constructors & Destructors */
		Dog(void);
		Dog(std::string const &type);
		Dog(Dog const &copy);
		~Dog(void);

		/* Basic Operators */
		Dog const	&operator=(Dog const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		void				setType(std::string const &type);

		/* Main Member Functions */
		void	makeSound(void) const;
};

class Cat: public Animal
{
	private:
		std::string	_type;
	public:
		/* Constructors & Destructors */
		Cat(void);
		Cat(std::string const &type);
		Cat(Cat const &copy);
		~Cat(void);

		/* Basic Operators */
		Cat const	&operator=(Cat const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		void				setType(std::string const &type);

		/* Main Member Functions */
		void	makeSound(void) const;
};
