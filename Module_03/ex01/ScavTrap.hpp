
#ifndef SCAVTRAP_HPP

# define SCAVTRAP_HPP

# include "ClapTrap.hpp"

class ScavTrap: public ClapTrap
{
	public:
		/* Constructors & Destructors */
		ScavTrap(void);
		ScavTrap(std::string const &name);
		ScavTrap(ScavTrap const &copy);
		~ScavTrap(void);

		/* Basic Operators */
		ScavTrap	&operator=(ScavTrap const &copy);

		/* Main Member Functions */
		void	guardGate(void);
};

#endif
