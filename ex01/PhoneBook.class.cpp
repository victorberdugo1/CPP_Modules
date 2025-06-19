/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.class.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 09:30:42 by victor            #+#    #+#             */
/*   Updated: 2025/06/19 12:29:02 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PhoneBook.class.hpp"

PhoneBook::PhoneBook() : _count(0) {}
PhoneBook::~PhoneBook() {}

std::string PhoneBook::truncateField(const std::string& str) {
	return str.length() > 10 ? str.substr(0, 9) + "." : str;
}

void PhoneBook::addContact() {
	std::string input;
	Contact new_contact;
	const char* prompts[5] = {
		"First Name",
		"Last Name",
		"Nickname",
		"Phone Number",
		"Darkest Secret"
	};

	for (int i = 0; i < 5; ++i) {
		do {
			std::cout << prompts[i] << ": ";
			std::getline(std::cin, input);
		} while (input.empty());

		if (i == 0)
			new_contact.setFirstName(input);
		else if (i == 1)
			new_contact.setLastName(input);
		else if (i == 2)
			new_contact.setNickname(input);
		else if (i == 3)
			new_contact.setPhoneNumber(input);
		else if (i == 4)
			new_contact.setDarkestSecret(input);
	}

	if (_count < 8) {
		_contact[_count] = new_contact;
		_count++;
	} else {
		for (int i = 1; i < 8; ++i)
			_contact[i - 1] = _contact[i];
		_contact[7] = new_contact;
	}
}

void PhoneBook::searchContact() const {
	if (_count == 0) {
		std::cout << "No contacts available" << std::endl;
		return;
	}
	std::cout << std::setw(10) << "Index" << "|"
		<< std::setw(10) << "First Name" << "|"
		<< std::setw(10) << "Last Name" << "|"
		<< std::setw(10) << "Nickname" << std::endl;
	for (int i = 0; i < _count; ++i) {
		std::cout << std::setw(10) << i << "|"
			<< std::setw(10) << truncateField(_contact[i].getFirstName()) << "|"
			<< std::setw(10) << truncateField(_contact[i].getLastName())  << "|"
			<< std::setw(10) << truncateField(_contact[i].getNickname())  << std::endl;
	}
	std::string idx_str;
	std::cout << "Enter index: ";
	std::getline(std::cin, idx_str);
	std::stringstream ss(idx_str);
	int idx;
	if (!(ss >> idx)) {
		std::cout << "Invalid index" << std::endl;
		return;
	}
	if (idx < 0 || idx >= _count) {
		std::cout << "Index out of range" << std::endl;
		return;
	}
	const Contact& c = _contact[idx];
	std::cout << "First Name    : " << c.getFirstName()    << std::endl
		<< "Last Name     : " << c.getLastName()     << std::endl
		<< "Nickname      : " << c.getNickname()     << std::endl
		<< "Phone Number  : " << c.getPhoneNumber()  << std::endl
		<< "Darkest Secret: " << c.getDarkestSecret()<< std::endl;
}

