#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Matrix.h"
#include "Vector.h"

namespace a3d
{
	class Camera
	{
	public:
		Camera();
		Camera(const Vertex4f& position, float xRotation, float yRotation, float zRotation);

		void setTarget(Vertex4f target);

		void moveForward(float distance);

		void rotate(const Matrix4f& m);
		
		void rotateX(float rad);
		void rotateY(float rad);
		void rotateZ(float rad);

		Vertex4f getPosition() const;
		void setPosition(const Vertex4f& position, bool relative = false);

		const Matrix4f& getRotation();
		float getRotationX();
		float getRotationY();
		float getRotationZ();
		void setRotation(float xRotation, float yRotation, float zRotation, bool relative = false);

	private:
		Vertex4f _position;
		Matrix4f _rotation;
		float _rotationX;
		float _rotationY;
		float _rotationZ;
	};
}

#endif