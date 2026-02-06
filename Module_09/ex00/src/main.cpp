/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:37:36 by vberdugo          #+#    #+#             */
/*   Updated: 2026/02/06 11:37:39 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

#include <iostream>
#include <fstream>
#include "BitcoinExchange.hpp"

int main(int argc, char **argv) {
  if (argc != 2)
    return (std::cout << "Usage: ./btc input.txt" << std::endl, 0);
  std::ifstream inputFile(argv[1]);
  if (!inputFile.is_open())
    return (std::cout << "Couldn't open the file " << argv[1] << std::endl, 0);
  if (inputFile.peek() == std::ifstream::traits_type::eof())
    return (std::cout << "No content on the file " << argv[1] << std::endl, 0);
  BitcoinExchange wallet(argv[1]);
  inputFile.close();
}
