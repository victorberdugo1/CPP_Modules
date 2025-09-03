/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DogCat.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 15:24:36 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/03 15:25:12 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DogCat.hpp"

/* Dog */
Dog::Dog(void): type_("Dog")
{
	std::cout << "Dog from Animal " << Animal::type_ << " created with default constructor." << std::endl;
}

Dog::Dog(std::string const &type): Animal(type), type_("Dog")
{
	std::cout << "Dog from Animal " << Animal::type_ << " created." << std::endl;
}

Dog::Dog(Dog const &copy): Animal(copy)
{
	*this = copy;
	std::cout << "Dog from Animal " << Animal::type_ << " copied." << std::endl;
}

Dog::~Dog(void)
{
	std::cout << "Dog from Animal " << Animal::type_ << " destroyed." << std::endl;
}

Dog const	&Dog::operator=(Dog const &copy)
{
	std::cout << "Assignment operator for Dog from Animal " << Animal::type_ << " called." << std::endl;
	Animal::operator=(copy);
	return (*this);
}

std::string const	&Dog::getType(void) const
{
	return (this->type_);
}

void	Dog::setType(const std::string &type)
{
	this->type_ = type;
}

void	Dog::makeSound(void) const
{
	std::cout << "Dog From Animal " << Animal::type_ << " goes Bark!" << std::endl;
}
/* Cat */
Cat::Cat(void): type_("Cat")
{
	std::cout << "Cat from Animal " << Animal::type_ << " created with default constructor." << std::endl;
}

Cat::Cat(std::string const &type):Animal(type), type_("Cat")
{
	std::cout << "Cat from Animal " << Animal::type_ << " created." << std::endl;
}

Cat::Cat(Cat const &copy): Animal(copy)
{
	*this = copy;
	std::cout << "Cat from Animal " << Animal::type_ << " copied." << std::endl;
}

Cat::~Cat(void)
{
	std::cout << "Cat from Animal " << Animal::type_ << " destroyed." << std::endl;
}

Cat const	&Cat::operator=(Cat const &copy)
{
	std::cout << "Assignment operator for Cat from Animal " << Animal::type_ << " called." << std::endl;
	Animal::operator=(copy);
	return (*this);
}

std::string const	&Cat::getType(void) const
{
	return (this->type_);
}

void	Cat::setType(const std::string &type)
{
	this->type_ = type;
}

void	Cat::makeSound(void) const
{
	std::cout << "Cat From Animal " << Animal::type_ << " goes Meow!" << std::endl;
}
