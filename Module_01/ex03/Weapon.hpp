/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Weapon.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 13:01:50 by vberdugo          #+#    #+#             */
/*   Updated: 2025/08/07 13:01:54 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEAPON_HPP

# define WEAPON_HPP

# include <iostream>

class	Weapon
{
	private:
		std::string	_type;
	public:
		Weapon(std::string type);
		~Weapon(void);
		const std::string&	getType(void);
		void		setType(std::string type);
	
};

#endif
