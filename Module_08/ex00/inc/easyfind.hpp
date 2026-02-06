/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   easyfind.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:32:19 by vberdugo          #+#    #+#             */
/*   Updated: 2026/02/06 11:32:23 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EX00_EASYFIND_HPP
#define EX00_EASYFIND_HPP

#include <algorithm>
#include <exception>
#include <iostream>
#include <vector>

template <typename T> void easyfind(T container, int toBeFound);

class ValueNotFound : public std::exception {
  virtual const char *what() const throw();
};
#include "easyfind.tpp"
#endif // EX00_EASYFIND_HPP
