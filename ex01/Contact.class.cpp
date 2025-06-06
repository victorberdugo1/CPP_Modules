/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Contact.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 12:07:37 by vberdugo          #+#    #+#             */
/*   Updated: 2025/06/06 12:02:08 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Contact.class.hpp"

Contact::Contact(void) {

	std::cout << "Constructor Call" << std::endl;
	Contact::_index += 1;
	return ;
}

Contact::~Contact(void) {

	std::cout << "Destructor Call" << std::endl;
	Contact::_index -= 1;
	return ;
}

int	Contact::get_index( void ) {

	return Contact::_index;
}

int	Contact::_index = 0;
