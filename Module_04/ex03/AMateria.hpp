
#pragma once

#include <iostream>

class ICharacter;

class AMateria
{
	protected:
		std::string	_type;

		/* Constructors & Destructors */
		AMateria(std::string const &type);
		AMateria(AMateria const &copy);
	public:
		/* Constructors & Destructors */
		virtual ~AMateria(void);

		/* Basic Operators */
		AMateria const	&operator=(AMateria const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;

		/* Main Member Functions */
		virtual AMateria	*clone(void) const = 0;
		virtual void		use(ICharacter &target) = 0;
};
