#ifndef __PIXEL_H__
#define __PIXEL_H__

namespace a3d
{
	struct Pixel
	{
		Pixel(int r = 0, int g = 0, int b = 0, int a = 0) { this->r = r; this->g = g; this->b = b; this->a = a; }
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	};
}

#endif