/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IMateriaSource.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 13:24:19 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/05 13:24:21 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IMateriaSource.hpp"

IMateriaSource::IMateriaSource(void)
{
	std::cout << "IMateriaSource object created with default constructor" << std::endl;
}

IMateriaSource::IMateriaSource(IMateriaSource const &copy)
{
	*this = copy;
	std::cout << "IMateriaSource object copied" << std::endl;
}

IMateriaSource const	&IMateriaSource::operator=(IMateriaSource const &copy)
{
	(void)copy;
	std::cout << "IMateriaSource Assignment Operator Called" << std::endl;
	return (*this);
}
