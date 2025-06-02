/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.class.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 09:30:42 by victor            #+#    #+#             */
/*   Updated: 2025/06/02 11:45:57 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "PhoneBook.class.hpp"

PhoneBook::PhoneBook(void) {

	std::cout << "Constructor Call" << std::endl;
	return ;
}

PhoneBook::~PhoneBook(void) {

	std::cout << "Destructor Call" << std::endl;
	return ;
}
