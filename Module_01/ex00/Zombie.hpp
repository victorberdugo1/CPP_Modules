/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Zombie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 12:33:54 by vberdugo          #+#    #+#             */
/*   Updated: 2025/08/07 12:33:59 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ZOMBIE_HPP

# define ZOMBIE_HPP

# include <iostream>

/* Zombie Class definition */
class	Zombie
{
	private:
		std::string	_name;
	public:
		Zombie(std::string name);
		~Zombie(void);
		void	announce(void);
};

#endif
