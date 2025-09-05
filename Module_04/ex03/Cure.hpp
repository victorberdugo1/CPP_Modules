/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cure.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:23:14 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:23:16 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
