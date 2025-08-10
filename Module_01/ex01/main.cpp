/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 12:36:18 by vberdugo          #+#    #+#             */
/*   Updated: 2025/08/07 12:36:27 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Zombie.hpp"

/* Creates horde of zombies named 'name' */
Zombie*	zombieHorde(int N, std::string name);

int	main(void)
{
	int	size;

	size = 20;
	Zombie *horde = zombieHorde(size, "Zom");
	for (int i = 0; i < size; i++)
		horde[i].announce();
	delete [] horde;
}
