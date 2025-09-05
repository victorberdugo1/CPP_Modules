/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MateriaSource.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:25:40 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:25:46 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "IMateriaSource.hpp"

class MateriaSource: public IMateriaSource
{
	private:
		AMateria	*learnInventory_[4];
	public:
		/* Constructors & Destructors */
		MateriaSource(void);
		MateriaSource(MateriaSource const &copy);
		~MateriaSource(void);

		/* Basic Operators */
		MateriaSource const	&operator=(MateriaSource const &copy);

		/* Main Member Functions */
		virtual void		learnMateria(AMateria *materia);
		virtual AMateria*	createMateria(std::string const &type);
		int 				inLearnInventory(AMateria *materia);
};
