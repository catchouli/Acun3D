#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <il.h>
#include <string.h>

namespace a3d
{
	class Image
	{
	public:
		Image();
		Image(const char* filename);
		~Image();

		const int* getData() const;
		int getWidth() const;
		int getHeight() const;

		bool load(const char* filename);

		static int* loadImage(const char* filename, int* width = 0, int* height = 0);

	private:
		int* _data;
		int _width;
		int _height;

		static bool _ilInitialised;
	};
}

#endif