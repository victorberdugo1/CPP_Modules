/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Harl.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 12:08:50 by vberdugo          #+#    #+#             */
/*   Updated: 2025/08/10 12:22:10 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Harl.hpp"

Harl::Harl(void)
{
	std::cout << "A wild Harl has appeared!" << std::endl;
}

Harl::~Harl(void)
{
	std::cout << "Wild Harl flew away!" << std::endl;
}

void	Harl::debug(void)
{
	std::cout << MAGENTA << "[DEBUG]   " << DEFAULT;
	std::cout << "I love having extra bacon for my ";
	std::cout << "7XL-double-cheese-triple-pickle-special-ketchup burger.";
	std::cout << " I really do!" << std::endl;
}

void	Harl::info(void)
{
	std::cout << GRAY << "[INFO]    " << DEFAULT;
	std::cout << "I cannot believe adding extra bacon cost more money.";
	std::cout << " You didn’t put enough bacon in my burger!";
	std::cout << " If you did, I wouldn’t be asking for more!" << std::endl;
}

void	Harl::warning(void)
{
	std::cout << YELLOW << "[WARNING] " << DEFAULT;
	std::cout << "I think I deserve to have some extra bacon for free.";
	std::cout << " I’ve been coming here for years, whereas you started ";
	std::cout << "working here last month." << std::endl;
}

void	Harl::error(void)
{
	std::cout << RED << "[ERROR]   " << DEFAULT;
	std::cout << "This is unacceptable, I want to speak to the manager now.";
	std::cout << std::endl;
}

void	Harl::complain(std::string level)
{
	void		(Harl::*ptr_complain[4])(void) = {&Harl::debug, &Harl::info, &Harl::warning, &Harl::error};
	std::string	levels[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};

	for (int i = 0; i < 4; i++)
	{
		if (levels[i] == level)
		{
			(this->*ptr_complain[i])();
			break ;
		}
	}
}
