/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.class.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 11:29:18 by vberdugo          #+#    #+#             */
/*   Updated: 2025/06/08 14:24:05 by victor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHONEBOOK_CLASS_HPP
# define PHONEBOOK_CLASS_HPP

#include "Contact.class.hpp"
#include <iomanip>
#include <sstream>

class PhoneBook {
	public:
		PhoneBook(void);
		~PhoneBook(void);
		void addContact();
		void searchContact() const;
	private:
		Contact _contact[8];
		int _count;
		static std::string truncateField(const std::string& str);
};

#endif
