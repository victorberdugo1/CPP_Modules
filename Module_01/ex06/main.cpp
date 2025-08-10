/************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 12:11:16 by vberdugo          #+#    #+#             */
/*   Updated: 2025/08/10 12:26:33 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Harl.hpp"

int	karen_translate(char *argv)
{
	std::string	levels[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};

	for (int i = 0; i < 4; i++)
		if (levels[i] == argv)
			return (i);
	return (-1);
}

void	karen_switch(char *argv, Harl & k)
{
	switch (karen_translate(argv))
	{
		case 0:
			k.complain("DEBUG");
		case 1:
			k.complain("INFO");
		case 2:
			k.complain("WARNING");
		case 3:
			k.complain("ERROR");
			break ;
		default:
			std::cout << "[ Probably complaining about insignificant problems ]" << std::endl;
	}
}

int	main(int argc, char **argv)
{
	Harl k;

	if (argc != 2)
		std::cout << "[ Probably complaining about insignificant problems ]" << std::endl;
	else
		karen_switch(argv[1], k);
	return (0);
}
