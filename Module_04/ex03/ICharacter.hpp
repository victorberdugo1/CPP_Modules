
#pragma once

#include "AMateria.hpp"

class ICharacter
{
	protected:
		/* Constructors & Destructors */
		ICharacter(void);
		ICharacter(ICharacter const &copy);

		/* Basic Operators */
		ICharacter const	&operator=(ICharacter const &copy);
	public:
		/* Constructors & Destructors */
		virtual	~ICharacter() {}

		/* Getters & Setters */
		virtual std::string const	&getName() const = 0;

		/* Main Member Functions */
		virtual void	equip(AMateria *m) = 0;
		virtual void	unequip(int idx) = 0;
		virtual void	use(int idx, ICharacter &target) = 0;
};
