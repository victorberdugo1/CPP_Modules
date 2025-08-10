/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 12:30:05 by vberdugo          #+#    #+#             */
/*   Updated: 2025/08/07 12:31:06 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Zombie.hpp"

/* Returns pointer to Zombie object allocated with new */
Zombie	*newZombie(std::string name);

/* Creates new Zombie stored on the heap, which announces itself */
void	randomChump(std::string name);

int	main(void)
{
	Zombie	*z = newZombie("Victor");
	z->announce();
	randomChump("Brian");
	delete z;
}
