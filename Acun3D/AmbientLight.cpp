#include "AmbientLight.h"

namespace a3d
{
	AmbientLight::AmbientLight(float intensity, Colour colour)
		: Light(LightTypes::AMBIENT, colour), _intensity(intensity)
	{

	}
		
	float AmbientLight::getIntesity() const
	{
		return _intensity;
	}

	void AmbientLight::setIntensity(float intensity)
	{
		_intensity = intensity;
	}
}