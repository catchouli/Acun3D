#ifndef __RASTERISER_H__
#define __RASTERISER_H__

#include "Matrix.h"
#include "Vertex.h"
#include "Pixel.h"
#include "Colour.h"
#include "MD2_Model.h"
#include "Camera.h"

namespace a3d
{
	class Rasteriser
	{
	public:
		Rasteriser();
		Rasteriser(Pixel* pixelBuffer, int width, int height);

		void setPixel(int x, int y, int colour);

		void drawLine(int x1, int y1, int x2, int y2, int colour);
		void drawEllipse(int x0, int y0, int ra, int rb, int colour);
		void drawCircle(int x, int y, int r, int colour);

		void drawTriangle(float x1, float y1, float z1, Colour c1,
							float x2, float y2, float z2, Colour c2,
							float x3, float y3, float z3, Colour c3);

		void drawTriangle(float x1f, float y1f, float z1, float uoz1, float voz1, float zr1, Colour c1,
							float x2f, float y2f, float z2, float uoz2, float voz2, float zr2, Colour c2,
							float x3f, float y3f, float z3, float uoz3, float voz3, float zr3, Colour c3,
							unsigned int textureCount, const Image* textures);

		void drawTriangle(float x1, float y1, float z1, const Vertex& cam1, const Vector& n1,
							float x2, float y2, float z2, const Vertex& cam2, const Vector& n2,
							float x3, float y3, float z3, const Vertex& cam3, const Vector& n3, std::vector<Light*>& lights);

		void drawTriangle(float x1f, float y1f, float z1, const Vertex& cam1, float uoz1, float voz1, float rz1, const Vector& n1,
							float x2f, float y2f, float z2, const Vertex& cam2, float uoz2, float voz2, float rz2, const Vector& n2,
							float x3f, float y3f, float z3, const Vertex& cam3, float uoz3, float voz3, float rz3, const Vector& n3,
							unsigned int textureCount, const Image* textures, std::vector<Light*>& lights);

		void setTarget(Pixel* pixelBuffer, int _width, int _height);
		void beginScene(Pixel colour);
	private:
		Pixel* _pixelBuffer;
		float* _depthBuffer;
		int _width;
		int _height;
	};
}

#endif