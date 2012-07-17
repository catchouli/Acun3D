#include "Triangle.h"

namespace a3d
{
	Triangle::Triangle()
	{
		normals = 0;
	}

	Triangle::~Triangle()
	{
		if (normals)
			delete[] normals;
	}
}