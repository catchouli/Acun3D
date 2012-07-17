#include "PointLight.h"

namespace a3d
{
	PointLight::PointLight(Vector position, Colour colour)
		: Light(LightTypes::POINT, colour), _position(position)
	{

	}
		
	const Vector& PointLight::getPosition() const
	{
		return _position;
	}
}