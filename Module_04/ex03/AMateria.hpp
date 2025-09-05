/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AMateria.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:21:10 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:21:13 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

class ICharacter;

class AMateria
{
	protected:
		std::string	type_;

		/* Constructors & Destructors */
		AMateria(std::string const &type);
		AMateria(AMateria const &copy);
	public:
		/* Constructors & Destructors */
		virtual ~AMateria(void);

		/* Basic Operators */
		AMateria const	&operator=(AMateria const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;

		/* Main Member Functions */
		virtual AMateria	*clone(void) const = 0;
		virtual void		use(ICharacter &target) = 0;
};
