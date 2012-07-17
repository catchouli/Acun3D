#ifndef __AMBIENTLIGHT_H__
#define __AMBIENTLIGHT_H__

#include "Light.h"

namespace a3d
{
	class AmbientLight
		: public Light
	{
	public:
		AmbientLight(float intensity, Colour colour = Colour(1.0f, 1.0f, 1.0f));
		
		float getIntesity() const;
		void setIntensity(float intensity);

	private:
		float _intensity;
	};
}

#endif