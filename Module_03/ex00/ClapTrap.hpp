/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClapTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 13:18:50 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/02 13:28:04 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLAPTRAP_HPP

# define CLAPTRAP_HPP

# include <iostream>

class ClapTrap
{
	private:
		std::string	name_;
		int			hp_;
		int			energyPoints_;
		int			damage_;
	public:
		/* Constructors & Destructors */
		ClapTrap(void);
		ClapTrap(std::string const &name);
		ClapTrap(ClapTrap const &copy);
		~ClapTrap(void);
		
		/* Main Member Functions */
		void	attack(std::string const &target);
		void	takeDamage(unsigned int amount);
		void	beRepaired(unsigned int amount);

		/* Basic Operators */
		ClapTrap	&operator=(ClapTrap const &copy);

		/* Getters */
		std::string const	&getName(void) const;
		int const			&getHp(void) const;
		int const			&getEnergyPoints(void) const;
		int const			&getDamage(void) const;
};

#endif
