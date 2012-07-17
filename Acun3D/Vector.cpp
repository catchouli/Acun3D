#include "Vector.h"

namespace a3d
{
	Vector::Vector() : Matrix<float, 4, 1>()
	{

	}

	Vector::Vector(float x, float y, float z) : Matrix<float, 4, 1>()
	{
		(*this)(0, 0) = x;
		(*this)(1, 0) = y;
		(*this)(2, 0) = z;
		(*this)(3, 0) = 0;
	}

	Vector& Vector::operator= (const Vector& rhs)
	{
		Matrix<float, 4, 1>::operator=((Matrix<float, 4, 1>)rhs);

		return *this;
	}

	Vector& Vector::operator= (const Matrix<float, 4, 1>& rhs)
	{
		Matrix<float, 4, 1>::operator=(rhs);

		return *this;
	}
		
	Vector Vector::operator+ (const Vector& rhs) const
	{
		Vector v;
		
		v.setX(getX() + rhs.getX());
		v.setY(getY() + rhs.getY());
		v.setZ(getZ() + rhs.getZ());

		return v;
	}

	Vector Vector::operator- () const
	{
		Vector v;
		
		v.setX(-getX());
		v.setY(-getY());
		v.setZ(-getZ());

		return v;
	}

	Vector Vector::operator- (const Vertex4f& rhs) const
	{
		Vector v;
		
		v.setX(getX() - rhs(0, 0));
		v.setY(getY() - rhs(1, 0));
		v.setZ(getZ() - rhs(2, 0));

		return v;
	}

	Vector Vector::operator* (float rhs) const
	{
		Vector v;
		
		v.setX(getX() * rhs);
		v.setY(getY() * rhs);
		v.setZ(getZ() * rhs);

		return v;
	}

	Vector Vector::operator/ (float rhs) const
	{
		Vector v;
		
		v.setX(getX() / rhs);
		v.setY(getY() / rhs);
		v.setZ(getZ() / rhs);

		return v;
	}

	Vector& Vector::operator/= (float rhs)
	{
		for (int i = 0; i < 4; i++)
			(*this)(i, 0) /= rhs;

		return *this;
	}

	float Vector::getX() const
	{
		return (*this)(0, 0);
	}

	void Vector::setX(float x)
	{
		(*this)(0, 0) = x;
	}

	float Vector::getY() const
	{
		return (*this)(1, 0);
	}

	void Vector::setY(float y)
	{
		(*this)(1, 0) = y;
	}

	float Vector::getZ() const
	{
		return (*this)(2, 0);
	}

	void Vector::setZ(float z)
	{
		(*this)(2, 0) = z;
	}

	float Vector::dot(const Vector& rhs) const
	{
		return (getX() * rhs.getX()) + (getY() * rhs.getY()) + (getZ() * rhs.getZ());
	}

	Vector Vector::cross(const Vector& other) const
	{
		Vector v;

		v.setX(getY() * other.getZ() - getZ() * other.getY());
		v.setY(getZ() * other.getX() - getX() * other.getZ());
		v.setZ(getX() * other.getY() - getY() * other.getX());

		return v;
	}

	float Vector::length()
	{
		return sqrt(getX() * getX() + getY() * getY() + getZ() * getZ());
	}

	float Vector::lengthSquared()
	{
		return getX() * getX() + getY() * getY() + getZ() * getZ();
	}

	void Vector::normalise()
	{
		float len = length();
		
		setX(getX() / len);
		setY(getY() / len);
		setZ(getZ() / len);
	}

	Vector Vector::getNormalised() const
	{
		Vector v = *this;

		v.normalise();

		return v;
	}

	Vector operator* (const Matrix<float, 4, 4>& lhs, const Vector& rhs)
	{
		Vector m;

		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 1; ++x)
			{
				m(y, x) = 0;
				for (int i = 0; i < 4; ++i)
				{
					float test = lhs(y, i);
					float test2 = rhs(i, x);
					m(y, x) += lhs(y, i) * rhs(i, x);
				}
			}
		}

		return m;
	}
}