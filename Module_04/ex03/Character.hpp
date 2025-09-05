/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:22:45 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:22:47 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ICharacter.hpp"

class Character: public ICharacter
{
	private:
		std::string	name_;
		AMateria	*inventory_[4];
	public:
		/* Constructors & Destructors */
		Character(void);
		Character(std::string const &name);
		Character(Character const &copy);
		~Character(void);

		/* Basic Operators */
		Character const	&operator=(Character const &copy);

		/* Getters & Setters */
		std::string const	&getName(void) const;
		void				setName(std::string const &name);

		/* Main Member Functions */
		virtual void	equip(AMateria *m);
		virtual void	unequip(int idx);
		virtual void	use(int idx, ICharacter &target);
		int 			inInventory(AMateria *m);
};
