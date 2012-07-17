#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "Vector.h"

namespace a3d
{
	struct Triangle
	{
		Triangle();
		~Triangle();

		// Vertex indices
		short A, B, C;

		// Face normals (one per frame)
		Vector* normals;

		// Texture coordinate indicies
		short AT, BT, CT;
	};
}

#endif