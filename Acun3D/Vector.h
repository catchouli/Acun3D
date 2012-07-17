#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "Matrix.h"

namespace a3d
{
	class Vector
		: public Matrix<float, 4, 1>
	{
	public:
		Vector();
		Vector(float x, float y, float z);
		
		Vector& operator= (const Vector& rhs);
		Vector& operator= (const Vector4f& rhs);
		Vector operator+ (const Vector& rhs) const;
		Vector operator- () const;
		Vector operator- (const Vertex4f& rhs) const;
		Vector operator* (float rhs) const;
		Vector operator/ (float rhs) const;
		Vector& operator/= (float rhs);
		const Vector& operator/= (float rhs) const;
		
		float getX() const;
		void setX(float x);
		float getY() const;
		void setY(float y);
		float getZ() const;
		void setZ(float z);

		float dot(const Vector& other) const;
		Vector cross(const Vector& other) const;

		float length();
		float lengthSquared();
		void normalise();
		Vector getNormalised() const;
	};

	Vector operator* (const Matrix<float, 4, 4>& lhs, const Vector& rhs);
}

#endif