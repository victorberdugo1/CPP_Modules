
#ifndef FRAGTRAP_HPP

# define FRAGTRAP_HPP

# include "ScavTrap.hpp"

class FragTrap: public virtual ClapTrap
{
	public:
		/* Constructors & Destructors */
		FragTrap(void);
		FragTrap(std::string const &name);
		FragTrap(FragTrap const &copy);
		~FragTrap(void);

		/* Basic Operators */
		FragTrap	&operator=(FragTrap const &copy);

		/* Main Member Functions */
		void	highFivesGuys(void);
};

#endif
