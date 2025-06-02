/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Contact.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 12:07:37 by vberdugo          #+#    #+#             */
/*   Updated: 2025/06/02 12:08:31 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Contact.class.hpp"

Contact::Contact(void) {

	std::cout << "Constructor Call" << std::endl;
	return ;
}

Contact::~Contact(void) {

	std::cout << "Destructor Call" << std::endl;
	return ;
}
