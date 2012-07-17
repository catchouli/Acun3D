#include "Colour.h"

namespace a3d
{
	Colour::Colour(float r, float g, float b)
	{
		setColour(r, g, b);
	}

	uint32_t Colour::toInt()
	{
		unsigned char c[4];
		c[0] = (unsigned char)(_b + 0.5f);
		c[1] = (unsigned char)(_g + 0.5f);
		c[2] = (unsigned char)(_r + 0.5f);

		return *(int*)&c;
	}

	void Colour::setColour(float r, float g, float b)
	{
		_r = r;
		_g = g;
		_b = b;
	}

	void Colour::clamp(float f)
	{
		if (_r > f)
			_r = f;
		if (_g > f)
			_g = f;
		if (_b > f)
			_b = f;
	}

	Colour& Colour::operator+= (const Colour& other)
	{
		_r += other._r;
		_g += other._g;
		_b += other._b;

		return *this;
	}

	Colour Colour::operator+ (const Colour& other)
	{
		Colour c = *this;
		c += other;

		return c;
	}

	Colour& Colour::operator-= (const Colour& other)
	{
		_r -= other._r;
		_g -= other._g;
		_b -= other._b;

		return *this;
	}

	Colour Colour::operator- (const Colour& other)
	{
		Colour c = *this;
		c -= other;

		return c;
	}

	Colour Colour::operator- ()
	{
		Colour colour;
		colour._r = -_r;
		colour._g = -_g;
		colour._b = -_b;

		return colour;
	}

	Colour& Colour::operator*= (const Colour& other)
	{
		_r *= other._r;
		_g *= other._g;
		_b *= other._b;

		return *this;
	}

	Colour& Colour::operator*= (const float other)
	{
		_r *= other;
		_g *= other;
		_b *= other;

		return *this;
	}

	Colour Colour::operator* (const Colour& other)
	{
		Colour c = *this;
		c *= other;

		return c;
	}

	Colour Colour::operator* (const float other)
	{
		Colour c = *this;
		c *= other;

		return c;
	}

	Colour& Colour::operator/= (const Colour& other)
	{
		_r /= other._r;
		_g /= other._g;
		_b /= other._b;

		return *this;
	}

	Colour& Colour::operator/= (const float other)
	{
		_r /= other;
		_g /= other;
		_b /= other;

		return *this;
	}

	Colour Colour::operator/ (const Colour& other)
	{
		Colour c = *this;
		c /= other;

		return c;
	}

	Colour Colour::operator/ (const float other)
	{
		Colour c = *this;
		c /= other;

		return c;
	}
}