#include "Spotlight.h"

namespace a3d
{
	Spotlight::Spotlight(Vector position, Vector direction, Colour colour, float fov, float exponent)
		: Light(LightTypes::SPOT, colour), _position(position), _direction(direction),
			_fov(fov), _exponent(exponent)
	{

	}
		
	const Vector& Spotlight::getPosition() const
	{
		return _position;
	}
		
	const Vector& Spotlight::getDirection() const
	{
		return _direction;
	}
		
	float Spotlight::getFOV() const
	{
		return _fov;
	}
		
	float Spotlight::getExponent() const
	{
		return _exponent;
	}
}