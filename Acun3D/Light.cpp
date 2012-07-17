#include "Light.h"

namespace a3d
{
	Light::Light(LightType type, Colour colour)
		: _type(type), _colour(colour)
	{
		_colourVector = colour / 255.0f;
	}
		
	const LightType& Light::getType() const
	{
		return _type;
	}
		
	const Colour& Light::getColour() const
	{
		return _colour;
	}
		
	const Colour& Light::getColourVector() const
	{
		return _colourVector;
	}
}
