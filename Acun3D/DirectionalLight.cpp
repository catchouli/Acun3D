#include "DirectionalLight.h"

namespace a3d
{
	DirectionalLight::DirectionalLight(Vector direction, Colour colour)
		: Light(LightTypes::DIRECTIONAL, colour), _direction(direction)
	{

	}
		
	const Vector& DirectionalLight::getDirection() const
	{
		return _direction;
	}
}