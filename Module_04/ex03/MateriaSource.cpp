/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MateriaSource.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:25:31 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:25:33 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MateriaSource.hpp"

MateriaSource::MateriaSource(void): learnInventory_()
{
	std::cout << "MateriaSource created with default constructor" << std::endl;
}

MateriaSource::~MateriaSource(void)
{
	for (int i = 0; i < 4; i++)
	{
		if (this->learnInventory_[i])
			delete this->learnInventory_[i];
	}
	std::cout << "MateriaSource destroyed" << std::endl;
}

MateriaSource::MateriaSource(MateriaSource const &copy): IMateriaSource(copy), learnInventory_()
{
	for (int i = 0; i < 4; i++)
	{
		if (copy.learnInventory_[i])
			this->learnInventory_[i] = copy.learnInventory_[i];
	}
	std::cout << "MateriaSource copied" << std::endl;
}

MateriaSource const	&MateriaSource::operator=(const MateriaSource &copy)
{
	(void)copy;
	return (*this);
}

void	MateriaSource::learnMateria(AMateria *materia)
{
	for (int i = 0; i < 4; i++)
	{
		if (materia && this->learnInventory_[i] == NULL)
		{
			if (this->inLearnInventory(materia))
				this->learnInventory_[i] = materia;
			else
				this->learnInventory_[i] = materia;
			std::cout << "Materia " << this->learnInventory_[i]->getType() << " learned at index " << i << std::endl;
			return ;
		}
	}
	if (materia)
		std::cout << "Cannot learn materia, inventory for MateriaSource is full!" << std::endl;
	else
		std::cout << "Cannot learn invalid materia" << std::endl;
	if (!this->inLearnInventory(materia))
		delete materia;
}

AMateria	*MateriaSource::createMateria(const std::string &type)
{
	for (int i = 0; i < 4; i++)
	{
		if (this->learnInventory_[i] && this->learnInventory_[i]->getType() == type)
			return (this->learnInventory_[i]->clone());
	}
	std::cout << "Cannot create materia, " << type << " is invalid!" << std::endl;
	return (0);
}

int MateriaSource::inLearnInventory(AMateria *materia)
{
	for (int i = 0; i < 4; i++)
	{
		if (this->learnInventory_[i] == materia)
			return (1);
	}
	return (0);
}
