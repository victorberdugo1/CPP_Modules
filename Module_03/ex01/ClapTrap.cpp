/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClapTrap.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 13:20:12 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/02 13:28:47 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClapTrap.hpp"

ClapTrap::ClapTrap(void): name_("default"), hp_(10), energyPoints_(10), damage_(0)
{
	std::cout << "ClapTrap " << this->name_ << " created with default constructor." << std::endl;
}

ClapTrap::ClapTrap(std::string const &name): name_(name), hp_(10), energyPoints_(10), damage_(0)
{
	std::cout << "ClapTrap " << this->name_ << " created." << std::endl;
}

ClapTrap::~ClapTrap(void)
{
	std::cout << "ClapTrap " << this->name_ << " destroyed." << std::endl;
}

ClapTrap::ClapTrap(ClapTrap const &copy)
{
	std::cout << "ClapTrap " << this->name_ << " copied." << std::endl;
	*this = copy;
}

void	ClapTrap::attack(std::string const &target)
{
	if (this->energyPoints_ && this->hp_ > 0)
	{
		std::cout << "ClapTrap " << this->name_ << " attacked " << target << ", causing " << this->damage_ << " point(s) of damage!" << std::endl;
		this->energyPoints_--;
	}
	if (this->hp_ <= 0)
		std::cout << "ClapTrap " << this->name_ << " is dead!" << std::endl;
	else if (!this->energyPoints_)
		std::cout << "ClapTrap " << this->name_ << " is out of energy points!" << std::endl;
}

void	ClapTrap::takeDamage(unsigned int amount)
{
	if (this->hp_ > 0)
	{
		std::cout << "ClapTrap " << this->name_ << " took " << amount << " point(s) of damage!" << std::endl;
		this->hp_ -= amount;
	}
	else
		std::cout << "STOP! ClapTrap " << this->name_ << " is already dead :(" << std::endl;
	if (this->hp_ < 0)
		this->hp_ = 0;

}

void	ClapTrap::beRepaired(unsigned int amount)
{
	if (this->hp_ > 0 && this->energyPoints_)
	{
		std::cout << "ClapTrap " << this->name_ << " healed " << amount << " point(s)." << std::endl;
		this->hp_ += amount;
		this->energyPoints_--;
	}
	if (this->hp_ <= 0)
		std::cout << "Cannot repair because: ClapTrap " << this->name_ << " is dead." << std::endl;
	else if (!this->energyPoints_)
		std::cout << "ClapTrap " << this->name_ << " is out of energy points!" << std::endl;
}

ClapTrap	&ClapTrap::operator=(const ClapTrap &copy)
{
	std::cout << "Assignment operator for ClapTrap called." << std::endl;
	this->name_ = copy.getName();
	this->hp_ = copy.getHp();
	this->energyPoints_ = copy.getEnergyPoints();
	this->damage_ = copy.getDamage();
	return (*this);
}

std::string const	&ClapTrap::getName(void) const
{
	return (this->name_);
}

int const	&ClapTrap::getHp(void) const
{
	return (this->hp_);
}

int const	&ClapTrap::getEnergyPoints(void) const
{
	return (this->energyPoints_);
}

int const	&ClapTrap::getDamage(void) const
{
	return (this->damage_);
}
