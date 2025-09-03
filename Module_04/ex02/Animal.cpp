/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 16:05:38 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/03 16:05:41 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Animal.hpp"

Animal::Animal(void): type_("undefined")
{
	std::cout << "Animal created with default constructor." << std::endl;
}

Animal::Animal(std::string const &type): type_(type)
{
	std::cout << "Animal created." << std::endl;
}

Animal::Animal(Animal const &copy)
{
	*this = copy;
	std::cout << "Animal copied." << std::endl;
}

Animal::~Animal(void)
{
	std::cout << "Animal destroyed." << std::endl;
}

Animal const	&Animal::operator=(Animal const &copy)
{
	this->type_ = copy.getType();
	std::cout << "Assignment operator for Animal called." << std::endl;
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
	std::cout << "Animal made a sound!" << std::endl;
}
