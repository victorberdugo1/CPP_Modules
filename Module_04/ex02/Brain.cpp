
#include "Brain.hpp"

Brain::Brain(void)
{
	std::cout << "Brain created with default constructor." << std::endl;
}

Brain::Brain(Brain const &copy)
{
	*this = copy;
	std::cout << "Brain copied." << std::endl;
}

Brain::~Brain(void)
{
	std::cout << "Brain destroyed." << std::endl;
}

Brain const	&Brain::operator=(Brain const &copy)
{
	std::copy(copy.ideas_, copy.ideas_ + 100, this->ideas_);
	std::cout << "Assignment operator for Brain called." << std::endl;
	return (*this);
}

std::string const	&Brain::getIdea(int const &index) const
{
	if (index >= 0 && index < 100)
		return (this->ideas_[index]);
	return (this->ideas_[0]);
}

void	Brain::setIdea(std::string const &idea, int const &index)
{
	if (index >= 0 && index < 100)
		this->ideas_[index] = idea;
}
