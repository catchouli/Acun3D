#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "Matrix.h"
#include "Vector.h"

namespace a3d
{
	class Vertex
		: public Vector
	{
	public:
		Vertex();
		Vertex(float x, float y, float z);

		const Vector getNormal() const;
		void setNormal(Vector normal);
		
		Vertex& operator= (const Vertex& rhs);
		Vertex operator+ (const Vertex& rhs) const;
		Vertex operator- (const Vertex& rhs) const;
		Vertex operator- () const;
		Vertex operator/ (float rhs) const;
		Vertex& operator/= (float rhs);
		const Vertex& operator/= (float rhs) const;
		Vertex operator* (float rhs) const;

	private:
		Vector _normal;
	};

	Vertex operator* (const Matrix<float, 4, 4>& lhs, const Vertex& rhs);
}

#endif