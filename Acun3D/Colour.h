#ifndef __COLOUR_H__
#define __COLOUR_H__

#include <stdint.h>

namespace a3d
{
	class Colour
	{
	public:
		Colour(float r = 0, float g = 0, float b = 0);

		uint32_t toInt();

		void setColour(float r = 0, float g = 0, float b = 0);

		void clamp(float f);
		
		Colour& operator+= (const Colour& other);
		Colour operator+ (const Colour& other);

		Colour& operator-= (const Colour& other);
		Colour operator- (const Colour& other);
		Colour operator- ();

		Colour& operator*= (const Colour& other);
		Colour& operator*= (const float other);
		Colour operator* (const Colour& other);
		Colour operator* (const float other);

		Colour& operator/= (const Colour& other);
		Colour& operator/= (const float other);
		Colour operator/ (const Colour& other);
		Colour operator/ (const float other);

		float _b;
		float _g;
		float _r;
	private:
	};
}

#endif