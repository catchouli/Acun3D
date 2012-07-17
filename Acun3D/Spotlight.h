#ifndef __SPOTLIGHT_H__
#define __SPOTLIGHT_H__

#include "Light.h"

namespace a3d
{
	class Spotlight
		: public Light
	{
	public:
		Spotlight(Vector position, Vector direction, Colour colour, float fov, float exponent);
		
		const Vector& getPosition() const;
		const Vector& getDirection() const;

		float getFOV() const;
		float getExponent() const;

	private:
		Vector _position;
		Vector _direction;
		float _fov;
		float _exponent;
	};
}

#endif