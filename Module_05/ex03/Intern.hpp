/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Intern.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 14:45:03 by vberdugo          #+#    #+#             */
/*   Updated: 2025/11/25 14:45:05 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include "ShrubberyCreationForm.hpp"
#include "RobotomyRequestForm.hpp"
#include "PresidentialPardonForm.hpp"

class Form;

class Intern
{
	public:
		/* Constructors & Destructors */
		Intern(void);
		Intern(Intern const &copy);
		~Intern(void);

		/* Basic Operators */
		Intern const	&operator=(Intern const &copy);

		/* Main Member Functions */
		Form	*makeForm(std::string const &type, std::string const &target);
};
