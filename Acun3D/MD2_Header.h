#ifndef __MD2_HEADER_H__
#define __MD2_HEADER_H__

namespace a3d
{
	namespace md2
	{
		struct MD2_Header
		{
			int ident;				// Magic number ("IDP2")
			int version;			// Version (should be 8)

			int skinWidth;			// Texture width
			int skinHeight;			// Texture height

			int frameSize;			// Size in bytes of a frame

			int skinCount;			// Number of skins
			int vertexCount;		// Number of vertices
			int texCoordCount;		// Number of texture coordinates
			int triangleCount;		// Number of triangles
			int glcmdCount;			// Number of opengl commands
			int frameCount;			// Number of frames

			int skinOffset;			// Offset of skins in file
			int texCoordOffset;		// Offset of texture coordinates
			int triangleOffset;		// Offset of triangles
			int frameOffset;		// Offset of frames
			int glcmdOffset;		// Offset of opengl commands
			int endOffset;			// Offset of end of file
		};
	}
}

#endif