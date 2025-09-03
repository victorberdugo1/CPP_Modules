/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 15:14:14 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/03 15:17:42 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Animal.hpp"

Animal::Animal(void): type_("undefined")
{
	std::cout << "Animal " << this->type_ << " created with default constructor." << std::endl;
}

Animal::Animal(std::string const &type): type_(type)
{
	std::cout << "Animal " << this->type_ << " created." << std::endl;
}

Animal::Animal(Animal const &copy)
{
	*this = copy;
	std::cout << "Animal " << this->type_ << " copied." << std::endl;
}

Animal::~Animal(void)
{
	std::cout << "Animal " << this->type_ << " destroyed." << std::endl;
}

Animal const	&Animal::operator=(Animal const &copy)
{
	std::cout << "Assignment operator for Animal " << this->type_ << " called." << std::endl;
	this->type_ = copy.getType();
	return (*this);
}

std::string const	&Animal::getType(void) const
{
	return (this->type_);
}

void	Animal::setType(const std::string &type)
{
	this->type_ = type;
}

void	Animal::makeSound(void) const
{
	std::cout << "Animal " << this->type_ << " made a sound!" << std::endl;
}
