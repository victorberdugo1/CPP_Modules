/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   temp.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:20:32 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:20:35 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Animal.hpp"
#include "DogCat.hpp"

int	main(void)
{
	const Animal	*animals[10];
	int				i;

	i = 0;
	while (i < 10)
	{
		if (i < 5)
			animals[i] = new Dog();
		else
			animals[i] = new Cat();
		i++;
	}
	while (i)
	{
		delete animals[10 - i];
		i--;
	}
	Dog	a;
	Dog	b(a);
}
