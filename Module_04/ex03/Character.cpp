/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Character.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:21:19 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:21:24 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Character.hpp"

Character::Character(void): name_("default"), inventory_()
{
	std::cout << "Character created with default constructor" << std::endl;
}

Character::Character(std::string const &name): name_(name), inventory_()
{
	std::cout << "Character created" << std::endl;
}

Character::~Character(void)
{
	for (int i = 0; i < 4; i++)
	{
		if (this->inventory_[i])
			delete this->inventory_[i];
	}
	std::cout << "Character destroyed" << std::endl;
}

Character::Character(Character const &copy): ICharacter(copy), inventory_()
{
	this->name_ = copy.name_;
	for (int i = 0; i < 4; i++)
	{
		if (copy.inventory_[i])
			this->inventory_[i] = copy.inventory_[i];
	}
	std::cout << "Character copied" << std::endl;
}

Character const	&Character::operator=(const Character &copy)
{
	this->name_ = copy.name_;
	return (*this);
}

void	Character::equip(AMateria *m)
{
	for (int i = 0; i < 4; i++)
	{
		if (m && this->inventory_[i] == NULL)
		{
			if (this->inInventory(m))
				this->inventory_[i] = m->clone();
			else
				this->inventory_[i] = m;
			std::cout << "Materia " << this->inventory_[i]->getType() << " equipped to " << this->name_ << "'s inventory at index " << i << std::endl;
			return ;
		}
	}
	if (m)
		std::cout << "Cannot equip materia, " << this->name_ << "'s inventory is full!" << std::endl;
	else
		std::cout << "Cannot equip invalid materia" << std::endl;
	if (!this->inInventory(m))
		delete m;
}

void	Character::unequip(int idx)
{
	if (idx >= 0 && idx < 4 && this->inventory_[idx])
	{
		std::cout << "Unequipped " << this->inventory_[idx]->getType() << " from " << this->name_ << "'s inventory at index " << idx << std::endl;
		this->inventory_[idx] = NULL;
	}
	else if (idx < 0 || idx >= 4)
		std::cout << "Cannot unequip materia, invalid index " << idx << std::endl;
	else
		std::cout << "Cannot unequip materia, index " << idx << " is empty!" << std::endl;
}

void	Character::use(int idx, ICharacter &target)
{
	if (idx >= 0 && idx < 4 && this->inventory_[idx])
		this->inventory_[idx]->use(target);
	else if (idx < 0 || idx >= 4)
		std::cout << "Cannot use materia, invalid index " << idx << std::endl;
	else
		std::cout << "Cannot use materia, index " << idx << " is empty!" << std::endl;
}

int Character::inInventory(AMateria *m)
{
	for (int i = 0; i < 4; i++)
	{
		if (this->inventory_[i] == m)
			return (1);
	}
	return (0);
}

std::string const	&Character::getName(void) const
{
	return (this->name_);
}

void	Character::setName(std::string const &name)
{
	this->name_ = name;
}
