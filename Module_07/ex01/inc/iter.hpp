/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   iter.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:29:44 by vberdugo          #+#    #+#             */
/*   Updated: 2026/02/06 11:29:47 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ITER_HPP
#define ITER_HPP

#include <functional>

template <typename T, typename F>
void iter(T *arrayAddress, int arrayLength, void(*func)(F)) {
  for (int i = 0; i < arrayLength; i++)
    func(arrayAddress[i]);
}

#endif // ITER_HPP
