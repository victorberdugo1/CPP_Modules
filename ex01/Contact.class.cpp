/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Contact.class.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 12:07:37 by vberdugo          #+#    #+#             */
/*   Updated: 2025/06/08 14:24:33 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Contact.class.hpp"

Contact::Contact( void ) {
	return ;
}

Contact::~Contact( void ) {
	return ;
}

void	Contact::setFirstName(const std::string& fn) {
	_first_name = fn;
}

void	Contact::setLastName(const std::string& ln) {
	_last_name = ln;
}

void	Contact::setNickname(const std::string& nn) {
	_nickname = nn; 
}

void	Contact::setPhoneNumber(const std::string& pn)
{
	_phone_number = pn;
}

void	Contact::setDarkestSecret(const std::string& ds) {
	_darkest_secret = ds; 
}

std::string	Contact::getFirstName() const {
	return _first_name;
}

std::string	Contact::getLastName() const {
	return _last_name;
}

std::string	Contact::getNickname() const {
	return _nickname;
}

std::string	Contact::getPhoneNumber() const {
	return _phone_number;
}

std::string	Contact::getDarkestSecret() const {
	return _darkest_secret;
}
