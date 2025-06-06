/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.class.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 09:30:42 by victor            #+#    #+#             */
/*   Updated: 2025/06/03 11:50:50 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "PhoneBook.class.hpp"


PhoneBook::PhoneBook(void) {

	std::cout << "Constructor Call" << std::endl;

	//this->_contact[0]
	return ;
}

PhoneBook::~PhoneBook(void) {

	std::cout << "Destructor Call" << std::endl;
	return ;
}
