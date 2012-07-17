#ifndef __MD2_STRUCTURES_H__
#define __MD2_STRUCTURES_H__

// MD2 Header
#include "MD2_Header.h"

namespace a3d
{
	namespace md2
	{
		// MD2 Ident ("IDP2")
		#define MD2_IDENT 844121161

		// MD2 Version
		#define MD2_VERSION	8

		// Loading
		typedef float Vector[3];

		typedef char TextureName[64];

		struct Vertex
		{
			unsigned char position[3];
			unsigned char normalIndex;
		};

		struct TextureCoords
		{
			short s;
			short t;
		};

		struct Frame
		{
			float scale[3];
			float translate[3];
			char name[16];
			Vertex vertices[1];
		};

		struct Triangle
		{
			unsigned short vertex[3];
			unsigned short texCoordIndices[3];
		};

		// Animation
		struct Animation
		{
			int firstFrame;
			int lastFrame;
			int fps;
		};

		struct AnimationState
		{
			AnimationState()
			{
				firstFrame = 0;
				lastFrame = 0;
				fps = 0;

				curTime = 0;
				oldTime = 0;
				curInterpolation = 0;

				curFrame = 0;
				nextFrame = 0;
			}

			int firstFrame;
			int lastFrame;                                                                               
			int fps;

			long curTime;
			long oldTime;
			float curInterpolation;

			int curFrame;
			int nextFrame;
		};
	}
}

#endif