#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "LightTypes.h"
#include "Vector.h"
#include "Colour.h"

namespace a3d
{
	struct Light
	{
	public:
		const LightType& getType() const;
		const Colour& getColour() const;
		const Colour& getColourVector() const;

	protected:
		Light(LightType type, Colour colour);

	private:
		LightType _type;
		Colour _colour;
		Colour _colourVector;
	};
}

#endif