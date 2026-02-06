/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   whatever.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:28:39 by vberdugo          #+#    #+#             */
/*   Updated: 2026/02/06 11:28:46 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WHATEVER_HPP
#define WHATEVER_HPP

template <typename T>
void swap(T &num1, T &num2) {
    T tmp = num1;
    num1 =  num2;
    num2 = tmp;
};
template <typename T>
  T min(T &num1, T &num2) {
   return (num1 < num2 ? num1 : num2);
};
template <typename T>
  T max(T &num1, T &num2) {
  return (num1 > num2 ? num1 : num2);
};


#endif //WHATEVER_HPP
