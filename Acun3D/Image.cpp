#include "Image.h"

namespace a3d
{
	Image::Image()
	{
		_data = 0;
		_width = 0;
		_height = 0;
	}

	Image::Image(const char* filename)
	{
		_data = loadImage(filename, &_width, &_height);
	}

	Image::~Image()
	{
		if (_data != 0)
			delete[] _data;
	}

	const int* Image::getData() const
	{
		return _data;
	}

	int Image::getWidth() const
	{
		return _width;
	}

	int Image::getHeight() const
	{
		return _height;
	}

	bool Image::load(const char* filename)
	{
		_data = loadImage(filename, &_width, &_height);

		return (_data != 0);
	}

	int* Image::loadImage(const char* filename, int* width, int* height)
	{
		// Make sure DevIL is initialised
		if (_ilInitialised)
			ilInit();

		// Convert string to unicode string
		unsigned int length = strlen(filename) + 1;
		wchar_t* wFilename = new wchar_t[length];

		for (unsigned int i = 0; i < length; ++i)
		{
			wFilename[i] = filename[i];
		}

		// Load texture with DevIL
        ILuint texid;        
        ilGenImages(1, &texid);
        ilBindImage(texid);
        ILboolean success = ilLoadImage(wFilename);

		delete wFilename;

        if (success)
        {
			int imageWidth = ilGetInteger(IL_IMAGE_WIDTH);
			int imageHeight = ilGetInteger(IL_IMAGE_HEIGHT);

			// If pointer to width and height return values specified, store width and height to them
			if (width != 0 && height != 0)
			{
				*width = imageWidth;
				*height = imageHeight;
			}

			// Allocate storage for bitmap
            int* data = new int[imageWidth * imageHeight];
			
			// Copy bitmap from DevIL's buffer
            ilCopyPixels(0, 0, 0, imageWidth, imageHeight, 1, IL_BGRA, IL_UNSIGNED_BYTE, data);

			// Clean up DevIL's buffer
            ilDeleteImage(texid);

			return data;
        }

		// If image could not be loaded return a null pointer and set the width and height to 0
		if (width != 0 && height != 0)
		{
			*width = 0;
			*height = 0;
		}

        return 0;
	}

	bool Image::_ilInitialised = true;
}