/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DiamondTrap.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 13:18:02 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/03 13:39:31 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "FragTrap.hpp"
# include "ScavTrap.hpp"

class DiamondTrap: public ScavTrap, public FragTrap
{
	private:
		std::string	_name;
	public:
		/* Constructors & Destructors */
		DiamondTrap(void);
		DiamondTrap(std::string const &name);
		DiamondTrap(DiamondTrap const &copy);
		~DiamondTrap(void);

		/* Basic Operators */
		DiamondTrap	&operator=(DiamondTrap const &copy);

		/* Main Member Functions */
		void	whoAmI(void);
		void	attack(std::string const &target);

		/* Getters & Setters */
		std::string const	&getName(void);
		void				setName(std::string const &name);
};
