#include "MatrixIndexException.h"

const char* MatrixIndexException::what() const throw()
{
	return "Matrix index out of bounds.";
}