/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DogCat.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:19:24 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:19:26 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Animal.hpp"

class Dog: public Animal
{
	private:
		Brain		*brain_;
	public:
		/* Constructors & Destructors */
		Dog(void);
		Dog(Dog const &copy);
		~Dog(void);

		/* Basic Operators */
		Dog const	&operator=(Dog const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		Brain 				&getBrain(void) const;
		void				setType(std::string const &type);
		void				setBrain(Brain const &brain);

		/* Main Member Functions */
		void	makeSound(void) const;
};

class Cat: public Animal
{
	private:
		Brain		*brain_;
	public:
		/* Constructors & Destructors */
		Cat(void);
		Cat(Cat const &copy);
		~Cat(void);

		/* Basic Operators */
		Cat const	&operator=(Cat const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		Brain 				&getBrain(void) const;
		void				setType(std::string const &type);
		void				setBrain(Brain const &brain);

		/* Main Member Functions */
		void	makeSound(void) const;
};
