/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Wrong.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 15:20:27 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/03 15:20:31 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

class WrongAnimal
{
	protected:
		std::string	_type;
	public:
		/* Constructors & Destructors */
		WrongAnimal(void);
		WrongAnimal(std::string const &type);
		WrongAnimal(WrongAnimal const &copy);
		virtual ~WrongAnimal(void);

		/* Basic Operators */
		WrongAnimal const	&operator=(WrongAnimal const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		void				setType(std::string const &type);

		/* Main member functions */
		void	makeSound(void) const;
};

class WrongCat: public WrongAnimal
{
	private:
		std::string	_type;
	public:
		/* Constructors & Destructors */
		WrongCat(void);
		WrongCat(std::string const &type);
		WrongCat(WrongCat const &copy);
		~WrongCat(void);

		/* Basic Operators */
		WrongCat const	&operator=(WrongCat const &copy);

		/* Getters & Setters */
		std::string const	&getType(void) const;
		void				setType(std::string const &type);

		/* Main Member Functions */
		void	makeSound(void) const;
};
