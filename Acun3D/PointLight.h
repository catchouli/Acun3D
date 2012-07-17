#ifndef __POINTLIGHT_H__
#define __POINTLIGHT_H__

#include "Light.h"

namespace a3d
{
	class PointLight
		: public Light
	{
	public:
		PointLight(Vector position, Colour colour);
		
		const Vector& getPosition() const;

		Vector _position;
	private:
	};
}

#endif