# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    easyfind.tpp                                       :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/06 11:32:28 by vberdugo          #+#    #+#              #
#    Updated: 2026/02/06 11:32:42 by vberdugo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#include "easyfind.hpp"

template <typename T> void easyfind(T container, int toBeFound) {
  typename T::iterator it;
  it = std::find(container.begin(), container.end(), toBeFound);
  if (it == container.end())
    throw ValueNotFound();
  std::cout << "Found the value> " << toBeFound << " in the position > "
            << std::distance(container.begin(), it) << std::endl;
}

const char *ValueNotFound::what() const throw() {
  return "[Exception] The value was not found in the container";
}
