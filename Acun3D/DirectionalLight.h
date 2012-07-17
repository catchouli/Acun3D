#ifndef __DIRECTIONALLIGHT_H__
#define __DIRECTIONALLIGHT_H__

#include "Light.h"

namespace a3d
{
	class DirectionalLight
		: public Light
	{
	public:
		DirectionalLight(Vector direction, Colour colour);
		
		const Vector& getDirection() const;

	private:
		Vector _direction;
	};
}

#endif