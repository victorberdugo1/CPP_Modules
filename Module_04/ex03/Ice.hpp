/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ice.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:23:42 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:23:44 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
