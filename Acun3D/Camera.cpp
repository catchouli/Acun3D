#include "Camera.h"

namespace a3d
{
	Camera::Camera()
	{
		_rotation = Matrix4f::createIdentity();
	}

	Camera::Camera(const Vertex4f& position, float xRotation, float yRotation, float zRotation)
	{
		_position = position;
		_rotation = Matrix4f::createRotationX(xRotation) * Matrix4f::createRotationX(yRotation) * Matrix4f::createRotationX(zRotation);
	}
		
	Vertex4f Camera::getPosition() const
	{
		return _position;
	}

	void Camera::setPosition(const Vertex4f& position, bool relative)
	{
		if (relative)
			_position += position;
		else
			_position = position;
	}

	const Matrix4f& Camera::getRotation()
	{
		return _rotation;
	}

	float Camera::getRotationX()
	{
		return _rotationX;
	}

	float Camera::getRotationY()
	{
		return _rotationY;
	}

	float Camera::getRotationZ()
	{
		return _rotationZ;
	}

	void Camera::setRotation(float xRotation, float yRotation, float zRotation, bool relative)
	{
		_rotationX = xRotation;
		_rotationY = yRotation;
		_rotationZ = zRotation;

		if (relative)
			_rotation = _rotation * Matrix4f::createRotationX(xRotation) * Matrix4f::createRotationY(yRotation) * Matrix4f::createRotationZ(zRotation);
		else
			_rotation = Matrix4f::createRotationX(xRotation) * Matrix4f::createRotationY(yRotation) * Matrix4f::createRotationZ(zRotation);
	}

	void Camera::rotate(const Matrix4f& m)
	{
		_rotation = m * _rotation;
	}

	void Camera::rotateX(float rad)
	{
		setRotation(_rotationX + rad, _rotationY, _rotationZ);
	}

	void Camera::rotateY(float rad)
	{
		setRotation(_rotationX, _rotationY + rad, _rotationZ);
	}

	void Camera::rotateZ(float rad)
	{
		setRotation(_rotationX, _rotationY, _rotationZ + rad);
	}
}