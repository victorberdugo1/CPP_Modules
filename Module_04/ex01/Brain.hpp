/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Brain.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vberdugo <vberdugo@student.42barcelon      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 15:36:36 by vberdugo          #+#    #+#             */
/*   Updated: 2025/09/03 15:36:40 by vberdugo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

class Brain
{
	private:
		std::string	_ideas[100];
	public:
		/* Constructors & Destructors */
		Brain(void);
		Brain(Brain const &copy);
		~Brain(void);

		/* Basic Operators */
		Brain const	&operator=(Brain const &copy);

		/* Getters & Setters */
		std::string const	&getIdea(int const &index) const;
		void				setIdea(std::string const &idea, int const &index);
};
