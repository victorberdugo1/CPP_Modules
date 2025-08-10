
#pragma once

#include "ICharacter.hpp"

class Ice: public AMateria
{
	public:
		/* Constructors & Destructors */
		Ice(void);
		Ice(Ice const &copy);
		~Ice(void);

		/* Basic Operators */
		Ice const	&operator=(Ice const &copy);

		/* Main Member Functions */
		virtual AMateria	*clone(void) const;
		void				use(ICharacter &target);
};
