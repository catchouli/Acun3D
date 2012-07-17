#ifndef __MATRIXINDEXEXCEPTION_H__
#define __MATRIXINDEXEXCEPTION_H__

#include <exception>

class MatrixIndexException : public std::exception
{
	virtual const char* what() const throw();
};

#endif