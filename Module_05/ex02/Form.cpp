
#include "Form.hpp"
#include "Bureaucrat.hpp"

Form::Form(void): _name("default"), _isSigned(0), _signGrade(30), _execGrade(60) {}

Form::Form(std::string const &name, int const &signGrade, int const &execGrade): _name(name), _isSigned(0), _signGrade(signGrade), _execGrade(execGrade)
{
	if (this->_signGrade < 1)
		throw (Form::GradeTooHighException());
	else if (this->_signGrade > 150)
		throw (Form::GradeTooLowException());
	if (this->_execGrade < 1)
		throw (Form::GradeTooHighException());
	else if (this->_execGrade > 150)
		throw (Form::GradeTooLowException());
}

Form::Form(Form const &copy): _name(copy._name), _isSigned(copy._isSigned), _signGrade(copy._signGrade), _execGrade(copy._execGrade)
{
	*this = copy;
}

Form::~Form(void) {}

Form const	&Form::operator=(const Form &copy)
{
	this->_isSigned = copy._isSigned;
	return (*this);
}

std::string const	&Form::getName(void) const
{
	return (this->_name);
}

int const	&Form::getSignGrade(void) const
{
	return (this->_signGrade);
}

int const	&Form::getExecGrade(void) const
{
	return (this->_execGrade);
}

bool const	&Form::getIsSigned(void) const
{
	return (this->_isSigned);
}

void	Form::beSigned(Bureaucrat &bureaucrat)
{
	if (bureaucrat.getGrade() > this->_signGrade)
		throw (Form::GradeTooLowException());
	else
	{
		this->_isSigned = 1;
		std::cout << bureaucrat.getName() << " successfully signed " << this->_name << std::endl;
	}
}

void	Form::execute(Bureaucrat const &executor) const
{
	if (!this->_isSigned)
		throw (Form::FormNotSignedException());
	else if (executor.getGrade() > this->_signGrade)
		throw (Form::GradeTooLowException());
	else
	{
		std::cout << executor.getName() << " executes form: " << this->_name << std::endl;
		this->beExecuted(executor);
	}
}

char const	*Form::GradeTooHighException::what(void) const throw()
{
	return ("Grade is too high");
}

char const	*Form::GradeTooLowException::what(void) const throw()
{
	return ("Grade is too low");
}

char const	*Form::FormNotSignedException::what(void) const throw()
{
	return ("Form not signed");
}

std::ostream	&operator<<(std::ostream &str, Form const &form)
{
	return (str << form.getName() << " form, signed: " << form.getIsSigned() << ", sign grade: " << form.getSignGrade() << ", exec grade: " << form.getExecGrade());
}
