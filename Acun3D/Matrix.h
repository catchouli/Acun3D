#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <iostream>
#include <cassert>
#include <stdarg.h>

#include "MatrixIndexException.h"

class Vertex;

/*
 * Generic matrix class
 * T is the type of the stored objects
 * M is the height, and N the width
 */

namespace a3d
{
	template <class T, int M, int N>
	class Matrix
	{
	public:
		Matrix();
		Matrix(int argc, ...);
		Matrix(const Matrix<T, M, N>& m);
		~Matrix();
		
		static Matrix<T, M, N> createZero();
		static Matrix<T, M, N> createIdentity();
		
		static Matrix<T, 4, 4> createPerspective(T d);
		static Matrix<T, 4, 4> createPerspective(double left, double right, double bottom,
												double top, double nearVal, double farVal);
		static Matrix<T, 4, 4> createPerspective(double left, double right, double bottom,
												double top, double nearVal, double farVal, float aspect);

		static Matrix<T, 3, 3> createTranslation(T x, T y);
		static Matrix<T, 4, 4> createTranslation(T x, T y, T z);

		static Matrix<T, 4, 4> createScale(T x, T y, T z);

		static Matrix<T, 4, 4> createRotationX(double theta);
		static Matrix<T, 4, 4> createRotationY(double theta);
		static Matrix<T, 4, 4> createRotationZ(double theta);
		
		T& operator() (int m, int n);
		const T& operator() (int m, int n) const;

		Matrix<T, M, N>& operator= (const Matrix<T, M, N>& rhs);	
		Matrix<T, M, N>& operator+= (const Matrix<T, M, N>& rhs);
		Matrix<T, M, N> operator+ (const Matrix<T, M, N>& rhs);
		Matrix<T, M, N>& operator-= (const Matrix<T, M, N>& rhs);
		Matrix<T, M, N> operator- ();
		Matrix<T, M, N> operator- (const Matrix<T, M, N>& rhs);
	
		template <int RN>
		Matrix<T, M, RN> operator* (const Matrix<T, N, RN>& rhs) const;
		Matrix<T, M, N>& operator*= (const Matrix<T, N, N>& rhs);

	private:
		T _data[M * N];
	};

	// Predefined types
	typedef Matrix<float, 2, 1> Vertex2f;
	typedef Matrix<float, 2, 1> Vector2f;
	typedef Matrix<float, 2, 2> Matrix2f;
	typedef Matrix<float, 3, 1> Vertex3f;
	typedef Matrix<float, 3, 1> Vector3f;
	typedef Matrix<float, 3, 3> Matrix3f;
	typedef Matrix<float, 4, 1> Vertex4f;
	typedef Matrix<float, 4, 1> Vector4f;
	typedef Matrix<float, 4, 4> Matrix4f;
	
	template <class T, int M, int N>
	void drawMatrix(const a3d::Matrix<T, M, N>& m)
	{
		for (int y = 0; y < M; y++)
		{
			for (int x = 0; x < N; x++)
			{
				std::cout << m(y, x) << "\t";
			}

			std::cout << std::endl;
		}

		std::cout << std::endl << std::endl;
	}

	template<class T, int M, int N>
	Matrix<T, M, N>::Matrix()
	{
		memset(_data, 0, sizeof(T) * M * N);
	}

	template<class T, int M, int N>
	Matrix<T, M, N>::Matrix(int argc, ...)
	{
		memset(_data, 0, sizeof(T) * M * N);

		if (argc > 0)
		{
			va_list list;

			va_start(list, argc);

			for (int i = 0; i < argc && i < M * N; ++i)
			{
				int arg = va_arg(list, int);

				_data[i] = (float)arg;
			}
		}
	}

	template <class T, int M, int N>
	Matrix<T, M, N>::Matrix(const Matrix<T, M, N>& m)
	{
		memcpy(_data, m._data, sizeof(T) * M * N);
	}

	template <class T, int M, int N>
	Matrix<T, M, N>::~Matrix()
	{

	}
	
	template <class T, int M, int N>
	Matrix<T, M, N> Matrix<T, M, N>::createZero()
	{
		return Matrix();
	}
	
	template <class T, int M, int N>
	Matrix<T, M, N> Matrix<T, M, N>::createIdentity()
	{
		Matrix m;

		// max = min(M, N)
		int max = M < N ? M : N;

		// Set up values where M and N are equal to one
		for (int i = 0; i < max; ++i)
		{
			m(i, i) = 1;
		}

		return m;
	}
	
	template <class T, int M, int N>
	Matrix<T, 4, 4> Matrix<T, M, N>::createPerspective(T d)
	{
		Matrix<T, 4, 4> m;
		
		m(0, 0) = d;
		m(1, 1) = d;
		m(2, 2) = d;
		m(3, 2) = 1;

		return m;
	}
	
	//template <class T, int M, int N>
	//Matrix<T, 4, 4> Matrix<T, M, N>::createPerspective(float fov, float aspect, float near, float far)
	//{
	//	Matrix<T, 4, 4> m;
	//	
	//	m(0, 0) = d;
	//	m(1, 1) = d;
	//	m(2, 2) = d;
	//	m(3, 2) = 1;

	//	return m;
	//}

	template<class T, int M, int N>
	Matrix<T, 4, 4> Matrix<T, M, N>::createPerspective(double left, double right, double bottom,
													double top, double nearVal, double farVal)
	{
		Matrix<T, 4, 4> m;

		m(0, 0) = (T)((2 * nearVal) / (right - left));
		m(0, 2) = (T)((right + left) / (right - left));
		m(1, 1) = (T)((2 * nearVal) / (top - bottom));
		m(1, 2) = (T)((top + bottom) / (top - bottom));
		m(2, 2) = (T)(-(farVal + nearVal) / (farVal - nearVal));
		m(2, 3) = (T)(-(2 * farVal * nearVal) / (farVal - nearVal));
		m(3, 2) = (T)(-1);

		return m;
	}

	template<class T, int M, int N>
	Matrix<T, 4, 4> Matrix<T, M, N>::createPerspective(double left, double right, double bottom,
													double top, double nearVal, double farVal, float aspect)
	{
		// Adjust frustum for aspect ratio
		double halffov = atan((top - bottom) * 0.5 / nearVal);
		top = tan(halffov) * nearVal;
		bottom = -top;

		left = aspect * top;
		right = aspect * bottom;

		return createPerspective(left, right, top, bottom, nearVal, farVal);
	}
	
	template <class T, int M, int N>
	Matrix<T, 3, 3> Matrix<T, M, N>::createTranslation(T x, T y)
	{
		Matrix<T, 3, 3> m;
		
		m(0, 0) = 1;
		m(1, 1) = 1;
		m(2, 2) = 1;
		m(0, 1) = x;
		m(1, 2) = y;

		return m;
	}
	
	template<class T, int HEIGHT, int WIDTH>
	Matrix<T, 4, 4> Matrix<T, HEIGHT, WIDTH>::createTranslation(T x, T y, T z)
	{
		Matrix<T, 4, 4> newMatrix = Matrix<T, 4, 4>::createIdentity();

		newMatrix(0, 3) = (T)(x);
		newMatrix(1, 3) = (T)(y);
		newMatrix(2, 3) = (T)(z);

		return newMatrix;
	}
	
	template<class T, int HEIGHT, int WIDTH>
	Matrix<T, 4, 4> Matrix<T, HEIGHT, WIDTH>::createScale(T x, T y, T z)
	{
		Matrix<T, 4, 4> m;
		
		m(0, 0) = x;
		m(1, 1) = y;
		m(2, 2) = z;
		m(3, 3) = 1;

		return m;
	}

	template<class T, int WIDTH, int HEIGHT>
	Matrix<T, 4, 4> Matrix<T, WIDTH, HEIGHT>::createRotationX(double theta)
	{
		Matrix<T, 4, 4> newMatrix = Matrix<T, 4, 4>::createIdentity();
	
		newMatrix(1, 1) = (T)(std::cos(theta));
		newMatrix(1, 2) = (T)(-std::sin(theta));
		newMatrix(2, 1) = (T)(std::sin(theta));
		newMatrix(2, 2) = (T)(std::cos(theta));
	
		return newMatrix;
	}

	template<class T, int WIDTH, int HEIGHT>
	Matrix<T, 4, 4> Matrix<T, WIDTH, HEIGHT>::createRotationY(double theta)
	{
		Matrix<T, 4, 4> newMatrix = Matrix<T, 4, 4>::createIdentity();
	
		newMatrix(0, 0) = (T)(std::cos(theta));
		newMatrix(0, 2) = (T)(std::sin(theta));
		newMatrix(2, 0) = (T)(-std::sin(theta));
		newMatrix(2, 2) = (T)(std::cos(theta));
	
		return newMatrix;
	}

	template<class T, int WIDTH, int HEIGHT>
	Matrix<T, 4, 4> Matrix<T, WIDTH, HEIGHT>::createRotationZ(double theta)
	{
		Matrix<T, 4, 4> newMatrix = Matrix<T, 4, 4>::createIdentity();
	
		newMatrix(0, 0) = (T)(std::cos(theta));
		newMatrix(0, 1) = (T)(-std::sin(theta));
		newMatrix(1, 0) = (T)(std::sin(theta));
		newMatrix(1, 1) = (T)(std::cos(theta));
	
		return newMatrix;
	}

	template <class T, int M, int N>
	T& Matrix<T, M, N>::operator() (int m, int n)
	{
		int MM = M;
		int NN = N;
		if (m >= 0 && m < M && n >= 0 && n < N)
			return _data[m * N + n];
		else
			throw MatrixIndexException();
	}

	template <class T, int M, int N>
	const T& Matrix<T, M, N>::operator() (int m, int n) const
	{
		int MM = M;
		int NN = N;
		if (m >= 0 && m < M && n >= 0 && n < N)
			return _data[m * N + n];
		else
			throw MatrixIndexException();
	}

	template <class T, int M, int N>
	Matrix<T, M, N>& Matrix<T, M, N>::operator= (const Matrix<T, M, N>& rhs)
	{
		memcpy(_data, rhs._data, sizeof(T) * M * N);

		return *this;
	}

	template <class T, int M, int N>
	Matrix<T, M, N>& Matrix<T, M, N>::operator+= (const Matrix<T, M, N>& rhs)
	{
		for (int i = 0; i < M * N; ++i)
		{
			_data[i] += rhs._data[i];
		}

		return *this;
	}

	template <class T, int M, int N>
	Matrix<T, M, N> Matrix<T, M, N>::operator+ (const Matrix<T, M, N>& rhs)
	{
		Matrix<T, M, N> m(*this);
		m += rhs;

		return m;
	}
	
	template <class T, int M, int N>
	Matrix<T, M, N> Matrix<T, M, N>::operator- ()
	{
		Matrix<T, M, N> m;

		for (int i = 0; i < M * N; ++i)
			m._data[i] = -m._data[i];

		return m;
	}

	template <class T, int M, int N>
	Matrix<T, M, N>& Matrix<T, M, N>::operator-= (const Matrix<T, M, N>& rhs)
	{
		for (int i = 0; i < M * N; ++i)
		{
			_data[i] -= rhs._data[i];
		}

		return *this;
	}

	template <class T, int M, int N>
	Matrix<T, M, N> Matrix<T, M, N>::operator- (const Matrix<T, M, N>& rhs)
	{
		Matrix<T, M, N> m(*this);
		m -= rhs;

		return m;
	}

	template <class T, int M, int N>
	Matrix<T, M, N>& Matrix<T, M, N>::operator*= (const Matrix<T, N, N>& rhs)
	{
		for (int y = 0; y < M; ++y)
		{
			for (int x = 0; x < N; ++x)
			{
				T count = 0;
				for (int i = 0; i < N; ++i)
				{
					T test = (*this)(y, i);
					T test2 = rhs(i, x);
					count += (*this)(y, i) * rhs(i, x);
				}

				(*this)(y, x) = count;
			}
		}

		return *this;
	}

	template <class T, int M, int N>
	template <int RN>
	Matrix<T, M, RN> Matrix<T, M, N>::operator* (const Matrix<T, N, RN>& rhs) const
	{
		Matrix<T, M, RN> m;

		for (int y = 0; y < M; ++y)
		{
			for (int x = 0; x < RN; ++x)
			{
				m(y, x) = 0;
				for (int i = 0; i < N; ++i)
				{
 					T l = (*this)(y, i);
					T r = rhs(i, x);
					T mul = l * r;
					m(y, x) += mul;
				}
			}
		}

		return m;
	}
}

#endif