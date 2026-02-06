/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 11:35:33 by vberdugo          #+#    #+#             */
/*   Updated: 2026/02/06 11:35:37 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MutantStack.hpp"

/*
int main()
{
  MutantStack<int> mstack;
  mstack.push(5);
  mstack.push(17);
  std::cout << mstack.top() << std::endl;
  mstack.pop();
  std::cout << mstack.size() << std::endl;
  mstack.push(3);
  mstack.push(5);
  mstack.push(737);
  //[...]
  mstack.push(0);
  MutantStack<int>::iterator it = mstack.begin();
  MutantStack<int>::iterator ite = mstack.end();
  ++it;
  --it;
  while (it != ite)
  {
    std::cout << *it << std::endl;
    ++it;
  }
  std::stack<int> s(mstack);
  return 0;
}*/

int main()
{
  MutantStack<char> cstack;
  cstack.push('a');
  cstack.push('a');
  std::cout << cstack.top() << std::endl;
  std::cout << cstack.size() << std::endl;

  std::cout << "Alphabet test" << std::endl;
  MutantStack<char> dstack;
  for(char c = 'a'; c <= 'z'; c++)
    dstack.push(c);
  std::cout << "Size of dstack is " << dstack.size() << std::endl;
  std::cout << "Top of dstack is " << dstack.top() << std::endl;

  MutantStack<char>::iterator it_a = dstack.begin();
  MutantStack<char>::iterator it_b = dstack.end();

  while(it_a != it_b)
  {
    std::cout << *it_a << std::endl;
    it_a++;
  }
}
