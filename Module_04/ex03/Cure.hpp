
#pragma once

#include "ICharacter.hpp"

class Cure: public AMateria
{
	public:
		/* Constructors & Destructors */
		Cure(void);
		Cure(Cure const &copy);
		~Cure(void);

		/* Basic Operators */
		Cure const	&operator=(Cure const &copy);

		/* Main Member Functions */
		virtual AMateria	*clone(void) const;
		void				use(ICharacter &target);
};
