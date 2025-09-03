/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DogCat.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 15:18:03 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/03 15:18:23 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Animal.hpp"

class Dog: public Animal
{
	private:
		std::string	type_;
	public:
		/* Constructors & Destructors */
		Dog(void);
		Dog(std::string const &type);
		Dog(Dog const &copy);
		~Dog(void);

		/* Basic Operators */
		Dog const	&operator=(Dog const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		void				setType(std::string const &type);

		/* Main Member Functions */
		void	makeSound(void) const;
};

class Cat: public Animal
{
	private:
		std::string	type_;
	public:
		/* Constructors & Destructors */
		Cat(void);
		Cat(std::string const &type);
		Cat(Cat const &copy);
		~Cat(void);

		/* Basic Operators */
		Cat const	&operator=(Cat const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		void				setType(std::string const &type);

		/* Main Member Functions */
		void	makeSound(void) const;
};
