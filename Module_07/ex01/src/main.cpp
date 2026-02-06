/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:29:29 by vberdugo          #+#    #+#             */
/*   Updated: 2026/02/06 11:29:35 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "iter.hpp"
#include <iostream>
#include <string>

void ft_addOneChar(char c) { std::cout << c << std::endl; }

int main() {
  std::string str = "ola julio";
  char *copy = (char *)str.c_str();
  ::iter<char>(&copy[0], str.size(),
               ft_addOneChar);
  return 0;
}
