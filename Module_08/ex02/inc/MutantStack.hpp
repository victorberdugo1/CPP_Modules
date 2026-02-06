/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MutantStack.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:35:09 by vberdugo          #+#    #+#             */
/*   Updated: 2026/02/06 11:35:11 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EX02_MUTANTSTACK_HPP
#define EX02_MUTANTSTACK_HPP

#include <iostream>
#include <stack>
#include <vector>

template <typename T, typename C = std::deque<T> >
class MutantStack : public std::stack<T, C> {
public:
  /**
   * @brief Default constructor.
   */
  MutantStack();
  /**
   * @brief Copy constructor.
   * @param other The other MutantStack to copy.
   */
  MutantStack(const MutantStack &other);
  /**
   * @brief Copy assignment operator.
   * @param other The other MutantStack to assign.
   * @return A reference to the assigned MutantStack.
   */
  MutantStack &operator=(const MutantStack &other);
  /**
   * @brief Destructor.
   */
  ~MutantStack();

  typedef typename C::iterator iterator;

  iterator begin();
  iterator end();
};

#include "MutantStack.tpp"

#endif // EX02_MUTANTSTACK_HPP
