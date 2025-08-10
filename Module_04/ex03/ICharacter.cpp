
#include "ICharacter.hpp"

ICharacter::ICharacter(void)
{
	std::cout << "ICharacter object created with default constructor" << std::endl;
}

ICharacter::ICharacter(ICharacter const &copy)
{
	*this = copy;
	std::cout << "ICharacter object copied" << std::endl;
}

ICharacter const	&ICharacter::operator=(ICharacter const &copy)
{
	(void)copy;
	std::cout << "ICharacter Assignment Operator Called" << std::endl;
	return (*this);
}
