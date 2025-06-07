/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.class.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 09:30:42 by victor            #+#    #+#             */
/*   Updated: 2025/06/07 13:44:56 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PhoneBook.class.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

PhoneBook::PhoneBook() : _count(0) {}
PhoneBook::~PhoneBook() {}

std::string PhoneBook::truncateField(const std::string& str) {
    return str.length() > 10 ? str.substr(0, 9) + "." : str;
}

void PhoneBook::addContact() {
    if (_count >= 8) {
        std::cout << "PhoneBook is full" << std::endl;
        return;
    }
    std::string input;
    const char* prompts[5] = {"First Name", "Last Name", "Nickname", "Phone Number", "Darkest Secret"};
    for (int i = 0; i < 5; ++i) {
        do {
            std::cout << prompts[i] << ": ";
            std::getline(std::cin, input);
        } while (input.empty());
        switch (i) {
            case 0: _contact[_count].setFirstName(input); break;
            case 1: _contact[_count].setLastName(input);  break;
            case 2: _contact[_count].setNickname(input);  break;
            case 3: _contact[_count].setPhoneNumber(input); break;
            case 4: _contact[_count].setDarkestSecret(input); break;
        }
    }
    ++_count;
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

