/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelona.co  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/06 13:43:08 by vberdugo          #+#    #+#             */
/*   Updated: 2025/12/06 13:43:12 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ScalarConverter.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./convert <literal>" << std::endl;
        return 1;
    }

    ScalarConverter::convert(argv[1]);
    return 0;
}
