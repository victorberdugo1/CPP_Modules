
#include <iostream>

class Brain
{
	private:
		std::string	ideas_[100];
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
