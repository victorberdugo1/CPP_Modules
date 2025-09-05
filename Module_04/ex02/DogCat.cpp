/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DogCat.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:18:27 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:19:17 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DogCat.hpp"

Dog::Dog(void): Animal("Dog")
{
	this->brain_ = new Brain();
	std::cout << "Dog created with default constructor." << std::endl;
}

Dog::Dog(Dog const &copy): Animal(copy)
{
	this->brain_ = new Brain(*copy.brain_);
	std::cout << "Dog copied." << std::endl;
}

Dog::~Dog(void)
{
	delete this->brain_;
	std::cout << "Dog destroyed." << std::endl;
}

Dog const	&Dog::operator=(Dog const &copy)
{
	Animal::operator=(copy);
	*this->brain_ = *copy.brain_;
	std::cout << "Assignment operator for Dog called." << std::endl;
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

Brain &Dog::getBrain(void) const
{
	return (*this->brain_);
}

void	Dog::setBrain(Brain const &brain)
{
	*this->brain_ = brain;
}

void	Dog::makeSound(void) const
{
	std::cout << "Dog From Animal goes Bark!" << std::endl;
}

Cat::Cat(void): Animal("Cat")
{
	this->brain_ = new Brain();
	std::cout << "Cat created with default constructor." << std::endl;
}

Cat::Cat(Cat const &copy): Animal(copy)
{
	*this = copy;
	this->brain_ = new Brain(*copy.brain_);
	std::cout << "Cat copied." << std::endl;
}

Cat::~Cat(void)
{
	delete this->brain_;
	std::cout << "Cat destroyed." << std::endl;
}

Cat const	&Cat::operator=(Cat const &copy)
{
	Animal::operator=(copy);
	//*this->brain_ = *copy.brain_;
	std::cout << "Assignment operator for Cat called." << std::endl;
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
	std::cout << "Cat From Animal goes Meow!" << std::endl;
}

Brain &Cat::getBrain(void) const
{
	return (*this->brain_);
}

void	Cat::setBrain(Brain const &brain)
{
	*this->brain_ = brain;
}
