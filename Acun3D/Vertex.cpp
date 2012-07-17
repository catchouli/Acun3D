#include "Vertex.h"

namespace a3d
{
	Vertex::Vertex()
		: Vector()
	{
		
	}

	Vertex::Vertex(float x, float y, float z)
		: Vector(x, y, z)
	{

	}

	const Vector Vertex::getNormal() const
	{
		return _normal;
	}

	void Vertex::setNormal(Vector n)
	{
		_normal = n;
	}

	Vertex& Vertex::operator= (const Vertex& rhs)
	{
		Matrix<float, 4, 1>::operator=((Matrix<float, 4, 1>)rhs);
		_normal = rhs.getNormal();

		return *this;
	}

	Vertex Vertex::operator+ (const Vertex& rhs) const
	{
		Vertex v = *this;
		
		v.setX(v.getX() + rhs.getX());
		v.setY(v.getY() + rhs.getY());
		v.setZ(v.getZ() + rhs.getZ());

		return v;
	}

	Vertex Vertex::operator- (const Vertex& rhs) const
	{
		Vertex v = *this;
		
		v.setX(v.getX() - rhs.getX());
		v.setY(v.getY() - rhs.getY());
		v.setZ(v.getZ() - rhs.getZ());

		return v;
	}

	Vertex Vertex::operator- () const
	{
		Vertex v;
		
		v.setX(-getX());
		v.setY(-getY());
		v.setZ(-getZ());

		return v;
	}

	Vertex Vertex::operator* (float rhs) const
	{
		Vertex v;
		
		v(0, 0) = (*this)(0, 0) * rhs;
		v(1, 0) = (*this)(1, 0) * rhs;
		v(2, 0) = (*this)(2, 0) * rhs;
		v(3, 0) = (*this)(3, 0) * rhs;

		return v;
	}

	Vertex operator* (const Matrix<float, 4, 4>& lhs, const Vertex& rhs)
	{
		Vertex m = rhs;

		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 1; ++x)
			{
				m(y, x) = 0;
				for (int i = 0; i < 4; ++i)
				{
					m(y, x) += lhs(y, i) * rhs(i, x);
				}
			}
		}

		return m;
	}

	Vertex Vertex::operator/ (float rhs) const
	{
		Vertex v;
		
		v.setX(getX() / rhs);
		v.setY(getY() / rhs);
		v.setZ(getZ() / rhs);

		return v;
	}

	Vertex& Vertex::operator/= (float rhs)
	{
		for (int i = 0; i < 4; i++)
			(*this)(i, 0) /= rhs;

		return *this;
	}
}