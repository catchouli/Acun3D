#include <math.h>
#include <limits>

#include "Rasteriser.h"

#define SSE2

#ifdef SSE2
#define SSE
#include <emmintrin.h>
#endif

#ifdef SSE
#include <xmmintrin.h>
#endif

namespace a3d
{
	Rasteriser::Rasteriser()
	{
		_pixelBuffer = 0;
		_depthBuffer = 0;
	}

	Rasteriser::Rasteriser(Pixel* pixelBuffer, int width, int height)
	{
		_pixelBuffer = 0;
		_depthBuffer = 0;

		setTarget(pixelBuffer, width, height);
	}

	void Rasteriser::setPixel(int x, int y, int colour)
	{
		if (x >= 0 && y >= 0 && x < _width && y < _height)
			_pixelBuffer[y * _width + x] = colour;
	}

	template <class T>
	T min(T i, T j)
	{
		return (i < j ? i : j);
	}
	
	template <class T>
	T min(T i, T j, T k)
	{
		return min(i, min(j, k));
	}
	
	template <class T>
	T max(T i, T j)
	{
		return (i > j ? i : j);
	}
	
	template <class T>
	T max(T i, T j, T k)
	{
		return max(i, max(j, k));
	}
	
	template <class T>
	T clamp(T n, T min, T max)
	{
		if (n < min)
			n = min;
		if (n > max)
			n = max;

		return n;
	}

	void Rasteriser::setTarget(Pixel* pixelBuffer, int width, int height)
	{
		if (_depthBuffer != 0)
			delete[] _depthBuffer;

		_pixelBuffer = pixelBuffer;
		_depthBuffer = new float[width * height];
		_width = width;
		_height = height;

		beginScene(Pixel(255, 255, 255, 0));
	}

	/*
	 * Clears the colour and depth buffer
	 */
	void Rasteriser::beginScene(Pixel colour)
	{
		std::fill_n(_pixelBuffer, _width * _height, *(Pixel*)&colour);
		std::fill_n(_depthBuffer, _width * _height, std::numeric_limits<float>::infinity());
	}

	void Rasteriser::drawLine(int x1, int y1, int x2, int y2, int colour)
	{
		int dx = std::abs(x2 - x1);
		int dy = std::abs(y2 - y1);

		int sx;
		int sy;

		if (x1 < x2)
			sx = 1;
		else
			sx = -1;

		if (y1 < y2)
			sy = 1;
		else
			sy = -1;

		int err = dx - dy;

		while (true)
		{
			setPixel(x1, y1, colour);

			if (x1 == x2 && y1 == y2)
				break;

			int e2 = 2 * err;

			if (e2 > -dy)
			{
				err -= dy;
				x1 += sx;
			}
			
			if (e2 < dx)
			{
				err += dx;
				y1 += sy;
			}
		}
	}

	/*
	 * Calculates initial value, dx and dy for interpolation
	 * returns the initial value, stores the dx and dy in the last two arguments
	 */
	template <class T>
	T calculateInterpolants(float initialx, float initialy,
							float x1, float y1, T z1,
							float x2, float y2, T z2,
							float x3, float y3, T z3,
							T* dx, T* dy)
	{
		// Ax + By + Cz + D = 0 where z is any dimension common to the
		// three points of the triangle (z-coordinate, colour, texture coordinate)
		// Incrementally, the initial value of z is (A * -minX + B * -minY -D) / C,
		// the difference for each X is -A/C, and the difference for each Y is -B/C

		// Calculate plane equation coefficients
		T Ac = (z2 - z3) * y1 + (z3 - z1) * y2 + (z1 - z2) * y3;
		T Bc = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
		float C = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
		T Dc = - ((z3 * y2 - z2 * y3) * x1 + (z1 * y3 - z3 * y1) * x2 + (z2 * y1 - z1 * y2) * x3);

		// Calculate initial value, dx and dy
		T initial = (Ac * -initialx + Bc * -initialy - Dc) / C;
		T rdx = -Ac / C;
		T rdy = -Bc / C;

		// Return values
		*dx = rdx;
		*dy = rdy;

		return initial;		
	}
	
	/*
	 * Draws a triangle using half-space equations to determine area and a plane equation
	 * derivation to interpolate colour values
	 * Optimised with incremental calculations and SSE/SSE2 instructions
	 */
	void Rasteriser::drawTriangle(float x1f, float y1f, float z1, Colour c1,
							float x2f, float y2f, float z2, Colour c2,
							float x3f, float y3f, float z3, Colour c3)
	{
		if (_pixelBuffer)
		{
			const int y1 = (int)(16.0f * y1f + 0.5f);
			const int y2 = (int)(16.0f * y2f + 0.5f);
			const int y3 = (int)(16.0f * y3f + 0.5f);

			const int x1 = (int)(16.0f * x1f + 0.5f);
			const int x2 = (int)(16.0f * x2f + 0.5f);
			const int x3 = (int)(16.0f * x3f + 0.5f);

			// Precalculate half-space function coefficients (so they can be calculated incrementally in the loop)
			const int dy1 = x1 - x2;
			const int dy2 = x2 - x3;
			const int dy3 = x3 - x1;

			const int dx1 = y1 - y2;
			const int dx2 = y2 - y3;
			const int dx3 = y3 - y1;
			
			const int fdx1 = dx1 << 4;
			const int fdx2 = dx2 << 4;
			const int fdx3 = dx3 << 4;
			
			const int fdy1 = dy1 << 4;
			const int fdy2 = dy2 << 4;
			const int fdy3 = dy3 << 4;

			// Work out min and max X and Y
			int minX = (min((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int maxX = (max((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int minY = (min((int)y1, (int)y2, (int)y3) + 0xF) >> 4;
			int maxY = (max((int)y1, (int)y2, (int)y3) + 0xF) >> 4;

			// Make sure it's not outside of the screen
			if (minX < 0)
				minX = 0;
			if (minY < 0)
				minY = 0;
			if (maxX >= _width)
				maxX = _width - 1;
			if (maxY >= _height)
				maxY = _height - 1;
			
			// Calculate half-space initial values
			int check1 = (int)((dy1 * (minY << 4)) - (dy1 * y1) - (dx1 * (minX << 4)) + (dx1 * x1));
			int check2 = (int)((dy2 * (minY << 4)) - (dy2 * y2) - (dx2 * (minX << 4)) + (dx2 * x2));
			int check3 = (int)((dy3 * (minY << 4)) - (dy3 * y3) - (dx3 * (minX << 4)) + (dx3 * x3));

			// Extend values if required for fill convention purposes
			if (dx1 < 0 || (dx1 == 0 && dy1 > 0))
				check1++;
			if (dx2 < 0 || (dx2 == 0 && dy2 > 0))
				check2++;
			if (dx3 < 0 || (dx3 == 0 && dy3 > 0)) 
				check3++;

			// Calculate initial colour value and dx/dy
			Colour dxC;
			Colour dyC;
			Colour initialC = calculateInterpolants((float)minX, (float)minY, x1f, y1f, c1,
													x2f, y2f, c2, x3f, y3f, c3, &dxC, &dyC);

			// The same for z-coordinate for z-buffering
			float dxZ;
			float dyZ;
			float initialZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, z1,
													x2f, y2f, z2, x3f, y3f, z3, &dxZ, &dyZ);
						
#ifdef SSE
			__m128 checkf = { (float)check1, (float)check2, (float)check3 };

			// Load interpolants __m128s for fast floating point addition
			__m128 colourf = { initialC._b, initialC._g, initialC._r, initialZ };
			__m128 dxcf = { dxC._b, dxC._g, dxC._r, dxZ };
			__m128 dycf = { dyC._b, dyC._g, dyC._r, dyZ };
#endif

			// Calculate address offset of initial position in buffer
			unsigned int* buffer = (unsigned int*)_pixelBuffer + minY * _width;
			float* depthBuffer = _depthBuffer + minY * _width;

			for (int y = minY; y < maxY; y++)
			{
				// Create temporary copies of our incremental values because they'll be needed in the next iteration
				int check1Temp = check1;
				int check2Temp = check2;
				int check3Temp = check3;
				
				float z;
#ifdef SSE
				__m128 currentColourf = colourf;
#else
				z = initialZ;
				Colour currentC = initialC;
#endif

				for (int x = minX; x < maxX; x++)
				{
					if (check1Temp > 0 &&
						check2Temp > 0 &&
						check3Temp > 0)
					{
#ifdef SSE
						z = currentColourf.m128_f32[3];
#endif
						// If on top of screen
						if (depthBuffer[x] > z)
						{						
							depthBuffer[x] = z;

							int i1;
							int i2;
							int i3;

#ifdef SSE2
							// Scale the values to the range 0 .. 255.0f
							static const __m128 twofivefive = { 255.0f, 255.0f, 255.0f, 255.0f };
							__m128 colour = _mm_mul_ps(currentColourf, twofivefive);

							// Convert our floating point colours to 32-bit integers
							__m128i intsOut = _mm_cvtps_epi32(colour);

							// Get the low-order bytes of each component
							i1 = intsOut.m128i_i8[0];
							i2 = intsOut.m128i_i8[4];
							i3 = intsOut.m128i_i8[8];
#else
#ifdef SSE
							// Scale the values to the range 0 .. 255.0f
							static const __m128 twofivefive = { 255.0f, 255.0f, 255.0f, 255.0f };
							__m128 colour = _mm_mul_ps(currentColourf, twofivefive);

							float f = colour.m128_f32[0];		// Prepare float for conversion
							__asm fld f									// Push it onto the FLU stack
							__asm fistp i1								// Magic it to an int

							f = colour.m128_f32[1];
							__asm fld f
							__asm fistp i2

							f = colour.m128_f32[2];
							__asm fld f
							__asm fistp i3
#else
							// Scale the values to the range 0 .. 255.0f
							i1 = (int)(currentC._b * 255.0f);
							i2 = (int)(currentC._g * 255.0f);
							i3 = (int)(currentC._r * 255.0f);
#endif
#endif

							((unsigned char*)&buffer[x])[0] = i1;
							((unsigned char*)&buffer[x])[1] = i2;
							((unsigned char*)&buffer[x])[2] = i3;
						}
					}

					// Increment values in x
					check1Temp -= fdx1;
					check2Temp -= fdx2;
					check3Temp -= fdx3;

#ifdef SSE
					currentColourf = _mm_add_ps(currentColourf, dxcf);
#else
					currentC += dxC;
					z += dxZ;
#endif
				}

				buffer += _width;
				depthBuffer += _width;

				// Increment values in y

				check1 += fdy1;
				check2 += fdy2;
				check3 += fdy3;

#ifdef SSE
				colourf = _mm_add_ps(colourf, dycf);
#else
				initialC += dyC;
				initialZ += dyZ;
#endif
			}
		}
	}

	/*
	 * Draws a textured triangle with colour interpolation
	 * uoz = u / z
	 * yoz = y / z
	 * zr = 1 / z
	 */
	void Rasteriser::drawTriangle(float x1f, float y1f, float z1, float uoz1, float voz1, float zr1, Colour c1,
							float x2f, float y2f, float z2, float uoz2, float voz2, float zr2, Colour c2,
							float x3f, float y3f, float z3, float uoz3, float voz3, float zr3, Colour c3,
							unsigned int textureCount, const Image* textures)
	{
		if (_pixelBuffer)
		{
			const int y1 = (int)(16.0f * y1f + 0.5f);
			const int y2 = (int)(16.0f * y2f + 0.5f);
			const int y3 = (int)(16.0f * y3f + 0.5f);

			const int x1 = (int)(16.0f * x1f + 0.5f);
			const int x2 = (int)(16.0f * x2f + 0.5f);
			const int x3 = (int)(16.0f * x3f + 0.5f);

			// Precalculate half-space function coefficients (so they can be calculated incrementally in the loop)
			const int dy1 = x1 - x2;
			const int dy2 = x2 - x3;
			const int dy3 = x3 - x1;

			const int dx1 = y1 - y2;
			const int dx2 = y2 - y3;
			const int dx3 = y3 - y1;
			
			const int fdx1 = dx1 << 4;
			const int fdx2 = dx2 << 4;
			const int fdx3 = dx3 << 4;
			
			const int fdy1 = dy1 << 4;
			const int fdy2 = dy2 << 4;
			const int fdy3 = dy3 << 4;

			// Work out min and max X and Y
			int minX = (min((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int maxX = (max((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int minY = (min((int)y1, (int)y2, (int)y3) + 0xF) >> 4;
			int maxY = (max((int)y1, (int)y2, (int)y3) + 0xF) >> 4;

			// Make sure it's not outside of the screen
			if (minX < 0)
				minX = 0;
			if (minY < 0)
				minY = 0;
			if (maxX >= _width)
				maxX = _width - 1;
			if (maxY >= _height)
				maxY = _height - 1;
			
			// Calculate half-space initial values
			int check1 = (int)((dy1 * (minY << 4)) - (dy1 * y1) - (dx1 * (minX << 4)) + (dx1 * x1));
			int check2 = (int)((dy2 * (minY << 4)) - (dy2 * y2) - (dx2 * (minX << 4)) + (dx2 * x2));
			int check3 = (int)((dy3 * (minY << 4)) - (dy3 * y3) - (dx3 * (minX << 4)) + (dx3 * x3));

			// Extend values if required for fill convention purposes
			if (dx1 < 0 || (dx1 == 0 && dy1 > 0))
				check1++;
			if (dx2 < 0 || (dx2 == 0 && dy2 > 0))
				check2++;
			if (dx3 < 0 || (dx3 == 0 && dy3 > 0)) 
				check3++;

			// Calculate initial colour value and dx/dy
			Colour dxC;
			Colour dyC;
			Colour initialC = calculateInterpolants((float)minX, (float)minY, x1f, y1f, c1,
													x2f, y2f, c2, x3f, y3f, c3, &dxC, &dyC);

			// The same for z-coordinate for z-buffering
			float dxZ;
			float dyZ;
			float initialZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, z1,
													x2f, y2f, z2, x3f, y3f, z3, &dxZ, &dyZ);

			// U / Z interpolation
			float dxUOZ;
			float dyUOZ;
			float initialUOZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, uoz1,
												x2f, y2f, uoz2, x3f, y3f, uoz3, &dxUOZ, &dyUOZ);

			// V / Z interpolation
			float dxVOZ;
			float dyVOZ;
			float initialVOZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, voz1,
													x2f, y2f, voz2, x3f, y3f, voz3, &dxVOZ, &dyVOZ);

			// 1 / Z interpolation
			float dxOOZ;
			float dyOOZ;
			float initialOOZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, zr1,
													x2f, y2f, zr2, x3f, y3f, zr3, &dxOOZ, &dyOOZ);
						
#ifdef SSE
			__m128 checkf = { (float)check1, (float)check2, (float)check3 };

			// Load interpolants __m128s for fast floating point conversion
			__m128 colourf = { initialC._b, initialC._g, initialC._r, initialZ };
			__m128 dxcf = { dxC._b, dxC._g, dxC._r, dxZ };
			__m128 dycf = { dyC._b, dyC._g, dyC._r, dyZ };

			// Load UV interpolants too
			__m128 initialUVf = { initialUOZ, initialVOZ, initialOOZ, 0 };
			__m128 dxUVf = { dxUOZ, dxVOZ, dxOOZ, 0 };
			__m128 dyUVf = { dyUOZ, dyVOZ, dyOOZ, 0 };
#endif

			// Calculate address offset of initial position in buffer
			unsigned int* buffer = (unsigned int*)_pixelBuffer + minY * _width;
			float* depthBuffer = _depthBuffer + minY * _width;

			for (int y = minY; y < maxY; y++)
			{
				// Create temporary copies of our incremental values because they'll be needed in the next iteration
				int check1Temp = check1;
				int check2Temp = check2;
				int check3Temp = check3;

				float UOZ = initialUOZ;
				float VOZ = initialVOZ;
				float OOZ = initialOOZ;
				
				float z;
#ifdef SSE
				__m128 currentColourf = colourf;
				__m128 UVf = initialUVf;
#else
				z = initialZ;
				Colour currentC = initialC;
#endif

				for (int x = minX; x < maxX; x++)
				{
					if (check1Temp > 0 &&
						check2Temp > 0 &&
						check3Temp > 0)
					{
#ifdef SSE
						z = currentColourf.m128_f32[3];
#endif
						// If on top of screen
						if (depthBuffer[x] > z)
						{						
							depthBuffer[x] = z;

							int i1;
							int i2;
							int i3;

							// Perspective-corrected U and V coordinates
							int tU;
							int tV;

#ifdef SSE2
							// Calculate perspective-correct texture coordinates
							float f;

							f = UVf.m128_f32[0] / UVf.m128_f32[2];
							__asm fld f
							__asm fistp tU
							
							f = UVf.m128_f32[1] / UVf.m128_f32[2];
							__asm fld f
							__asm fistp tV
							
							if (tU < 0)
								tU = 0;
							if (tV < 0)
								tV = 0;
							if (tU >= textures[0].getWidth())
								tU = textures[0].getWidth() - 1;
							if (tV >= textures[0].getHeight())
								tV = textures[0].getHeight() - 1;

							// Get texture colour at current (U,V) and multiply with light colour
							unsigned char* texture = (unsigned char*)&textures[0].getData()[(tV * textures[0].getWidth()) + tU];
							__m128 textureColour = { texture[0], texture[1], texture[2], 0 };

							// Scale it to the range 0 .. 1.0f
							static const __m128 twofivefive = { 255.0f, 255.0f, 255.0f, 255.0f };
							textureColour = _mm_div_ps(textureColour, twofivefive);

							// Multiply with light colour
							__m128 colour = _mm_mul_ps(currentColourf, textureColour);

							// Multiply by 255.0f
							colour = _mm_mul_ps(colour, twofivefive);

							// Clamp to 255.0f
							colour = _mm_min_ps(colour, twofivefive);

							// Convert our floating point colours to 32-bit integers
							__m128i intsOut = _mm_cvtps_epi32(colour);

							// Get the low-order bytes of each component
							i1 = intsOut.m128i_i8[0];
							i2 = intsOut.m128i_i8[4];
							i3 = intsOut.m128i_i8[8];

#else
#ifdef SSE
							// Calculate perspective-correct texture coordinates
							float f;

							f = UVf.m128_f32[0] / UVf.m128_f32[2];
							__asm fld f
							__asm fistp tU
							
							f = UVf.m128_f32[1] / UVf.m128_f32[2];
							__asm fld f
							__asm fistp tV
							
							if (tU < 0)
								tU = 0;
							if (tV < 0)
								tV = 0;
							if (tU >= textures[0].getWidth())
								tU = textures[0].getWidth() - 1;
							if (tV >= textures[0].getHeight())
								tV = textures[0].getHeight() - 1;

							// Get texture colour at current (U,V) and multiply with light colour
							unsigned char* texture = (unsigned char*)&textures[0].getData()[(tV * textures[0].getWidth()) + tU];
							__m128 textureColour = { texture[0], texture[1], texture[2], 0 };

							// Dive it by 255.0f to get it in the range 0 .. 1.0f
							static const __m128 twofivefive = _mm_set_ps(255.0f, 255.0f, 255.0f, 255.0f);
							textureColour = _mm_div_ps(textureColour, twofivefive);

							// Blend light colour with texture colour by way of multiply
							__m128 colour = _mm_mul_ps(currentColourf, textureColour);

							// Multiply by 255.0f
							colour = _mm_mul_ps(colour, twofivefive);

							// Clamp to 255.0f
							colour = _mm_min_ps(colour, twofivefive);

							f = colour.m128_f32[0];						// Prepare float for conversion
							__asm fld f									// Push it onto the FLU stack
							__asm fistp i1								// Magic it to an int

							f = colour.m128_f32[1];
							__asm fld f
							__asm fistp i2

							f = colour.m128_f32[2];
							__asm fld f
							__asm fistp i3
#else
							Colour colour = currentC;

							// Calculate perspective-correct U and V
							tU = (int)(UOZ / OOZ);
							tV = (int)(VOZ / OOZ);
							
							if (tU < 0)
								tU = 0;
							if (tV < 0)
								tV = 0;
							if (tU >= textures[0].getWidth())
								tU = textures[0].getWidth() - 1;
							if (tV >= textures[0].getHeight())
								tV = textures[0].getHeight() - 1;

							unsigned char* texture = (unsigned char*)&textures[0].getData()[(tV * textures[0].getWidth()) + tU];
							Colour textureColour(texture[2], texture[1], texture[0]);
							textureColour /= 255.0f;

							colour *= textureColour;
							colour *= 255.0f;
							colour.clamp(255.0f);

							i1 = (int)colour._b;
							i2 = (int)colour._g;
							i3 = (int)colour._r;
#endif
#endif

							((unsigned char*)&buffer[x])[0] = i1;
							((unsigned char*)&buffer[x])[1] = i2;
							((unsigned char*)&buffer[x])[2] = i3;
						}
					}

					// Increment values in x
					check1Temp -= fdx1;
					check2Temp -= fdx2;
					check3Temp -= fdx3;

#ifdef SSE
					currentColourf = _mm_add_ps(currentColourf, dxcf);
					UVf = _mm_add_ps(UVf, dxUVf);
#else
					currentC += dxC;

					z += dxZ;

					// Interpolate u/z, v/z and 1/z
					UOZ += dxUOZ;
					VOZ += dxVOZ;
					OOZ += dxOOZ;
#endif
				}

				buffer += _width;
				depthBuffer += _width;

				// Increment values in y

				check1 += fdy1;
				check2 += fdy2;
				check3 += fdy3;


#ifdef SSE
				colourf = _mm_add_ps(colourf, dycf);
				initialUVf = _mm_add_ps(initialUVf, dyUVf);
#else
				initialC += dyC;

				initialZ += dyZ;

				initialUOZ += dyUOZ;
				initialVOZ += dyVOZ;
				initialOOZ += dyOOZ;
#endif
			}
		}
	}
	
	/*
	 * Draws a triangle and interpolates the normals to generate lighting per pixel
	 */
	void Rasteriser::drawTriangle(float x1f, float y1f, float z1, const Vertex& cam1, const Vector& n1,
							float x2f, float y2f, float z2, const Vertex& cam2, const Vector& n2,
							float x3f, float y3f, float z3, const Vertex& cam3, const Vector& n3, std::vector<Light*>& lights)
	{
		if (_pixelBuffer)
		{
			const int y1 = (int)(16.0f * y1f + 0.5f);
			const int y2 = (int)(16.0f * y2f + 0.5f);
			const int y3 = (int)(16.0f * y3f + 0.5f);

			const int x1 = (int)(16.0f * x1f + 0.5f);
			const int x2 = (int)(16.0f * x2f + 0.5f);
			const int x3 = (int)(16.0f * x3f + 0.5f);

			// Precalculate half-space function coefficients (so they can be calculated incrementally in the loop)
			const int dy1 = x1 - x2;
			const int dy2 = x2 - x3;
			const int dy3 = x3 - x1;

			const int dx1 = y1 - y2;
			const int dx2 = y2 - y3;
			const int dx3 = y3 - y1;
			
			const int fdx1 = dx1 << 4;
			const int fdx2 = dx2 << 4;
			const int fdx3 = dx3 << 4;
			
			const int fdy1 = dy1 << 4;
			const int fdy2 = dy2 << 4;
			const int fdy3 = dy3 << 4;

			// Work out min and max X and Y
			int minX = (min((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int maxX = (max((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int minY = (min((int)y1, (int)y2, (int)y3) + 0xF) >> 4;
			int maxY = (max((int)y1, (int)y2, (int)y3) + 0xF) >> 4;

			// Make sure it's not outside of the screen
			if (minX < 0)
				minX = 0;
			if (minY < 0)
				minY = 0;
			if (maxX >= _width)
				maxX = _width - 1;
			if (maxY >= _height)
				maxY = _height - 1;
			
			// Calculate half-space initial values
			int check1 = (int)((dy1 * (minY << 4)) - (dy1 * y1) - (dx1 * (minX << 4)) + (dx1 * x1));
			int check2 = (int)((dy2 * (minY << 4)) - (dy2 * y2) - (dx2 * (minX << 4)) + (dx2 * x2));
			int check3 = (int)((dy3 * (minY << 4)) - (dy3 * y3) - (dx3 * (minX << 4)) + (dx3 * x3));

			// Extend values if required for fill convention purposes
			if (dx1 < 0 || (dx1 == 0 && dy1 > 0))
				check1++;
			if (dx2 < 0 || (dx2 == 0 && dy2 > 0))
				check2++;
			if (dx3 < 0 || (dx3 == 0 && dy3 > 0)) 
				check3++;

			// Calculate initial normals
			Vector dxN;
			Vector dyN;
			Vector initialN = calculateInterpolants((float)minX, (float)minY, x1f, y1f, n1,
													x2f, y2f, n2, x3f, y3f, n3, &dxN, &dyN);

			// The same for z-coordinate for z-buffering
			float dxZ;
			float dyZ;
			float initialZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, z1,
													x2f, y2f, z2, x3f, y3f, z3, &dxZ, &dyZ);

			// Also camera space z needs to be interpolated for perspective correct textures/lighting
			// Camera-space z is too imprecise for z-buffering :<
			Vertex dxCam;
			Vertex dyCam;
			Vertex initialCam = calculateInterpolants((float)minX, (float)minY, x1f, y1f, cam1,
													x2f, y2f, cam2, x3f, y3f, cam3, &dxCam, &dyCam);
						
#ifdef SSE
			// Load interpolants __m128s for fast floating point addition
			__m128 normalf = { initialN.getX(), initialN.getY(), initialN.getZ(), initialZ };
			__m128 dxnf = { dxN.getX(), dxN.getY(), dxN.getZ(), dxZ };
			__m128 dynf = { dyN.getX(), dyN.getY(), dyN.getZ(), dyZ };

			// Temporary vector to store normal for calculations
			Vector currentNormal;
#endif

			// Calculate address offset of initial position in buffer
			unsigned int* buffer = (unsigned int*)_pixelBuffer + minY * _width;
			float* depthBuffer = _depthBuffer + minY * _width;

			// Temporary vertex to store position for calculations
			Vertex currentPosition;

			for (int y = minY; y < maxY; y++)
			{
				// Create temporary copies of our incremental values because they'll be needed in the next iteration
				int check1Temp = check1;
				int check2Temp = check2;
				int check3Temp = check3;
				
				float z;
				Vertex camSpacePos = initialCam;
#ifdef SSE
				__m128 currentNormalf = normalf;
#else
				z = initialZ;
				Vector currentNormal = initialN;
#endif

				for (int x = minX; x < maxX; x++)
				{
					if (check1Temp > 0 &&
						check2Temp > 0 &&
						check3Temp > 0)
					{
#ifdef SSE
						z = currentNormalf.m128_f32[3];
#endif
						// If on top of screen
						if (depthBuffer[x] >= z)
						{						
							depthBuffer[x] = z;
							
#ifdef SSE
							currentNormal.setX(currentNormalf.m128_f32[0]);
							currentNormal.setY(currentNormalf.m128_f32[1]);
							currentNormal.setZ(currentNormalf.m128_f32[2]);
#endif

							Colour c = md2::MD2_Model::calculateLights(camSpacePos, currentNormal, lights);

							int i1;
							int i2;
							int i3;

#ifdef SSE
							__m128 colours = { c._b, c._g, c._r, 0 };
#endif

#ifdef SSE2
							// Scale the values to the range 0 .. 255.0f
							static const __m128 twofivefive = { 255.0f, 255.0f, 255.0f, 255.0f };
							__m128 colour = _mm_mul_ps(colours, twofivefive);

							// Convert our floating point colours to 32-bit integers
							__m128i intsOut = _mm_cvtps_epi32(colour);

							// Get the low-order bytes of each component
							i1 = intsOut.m128i_i8[0];
							i2 = intsOut.m128i_i8[4];
							i3 = intsOut.m128i_i8[8];
#else
#ifdef SSE
							// Scale the values to the range 0 .. 255.0f
							static const __m128 twofivefive = { 255.0f, 255.0f, 255.0f, 255.0f };
							__m128 colour = _mm_mul_ps(colours, twofivefive);

							float f = colour.m128_f32[0];		// Prepare float for conversion
							__asm fld f									// Push it onto the FLU stack
							__asm fistp i1								// Magic it to an int

							f = colour.m128_f32[1];
							__asm fld f
							__asm fistp i2

							f = colour.m128_f32[2];
							__asm fld f
							__asm fistp i3
#else
							// Scale the values to the range 0 .. 255.0f
							i1 = (int)(c._b * 255.0f);
							i2 = (int)(c._g * 255.0f);
							i3 = (int)(c._r * 255.0f);
#endif
#endif

							((unsigned char*)&buffer[x])[0] = i1;
							((unsigned char*)&buffer[x])[1] = i2;
							((unsigned char*)&buffer[x])[2] = i3;
						}
					}

					// Increment values in x
					check1Temp -= fdx1;
					check2Temp -= fdx2;
					check3Temp -= fdx3;

					camSpacePos += dxCam;

#ifdef SSE
					currentNormalf = _mm_add_ps(currentNormalf, dxnf);
#else
					currentNormal += dxN;
					z += dxZ;
#endif
				}

				buffer += _width;
				depthBuffer += _width;

				// Increment values in y

				check1 += fdy1;
				check2 += fdy2;
				check3 += fdy3;

				initialCam += dyCam;

#ifdef SSE
				normalf = _mm_add_ps(normalf, dynf);
#else
				initialN += dyN;
				initialZ += dyZ;
#endif
			}
		}
	}
	
	/*
	 * Draws a textured triangle and interpolates the normals to generate lighting per pixel
	 */
	void Rasteriser::drawTriangle(float x1f, float y1f, float z1, const Vertex& cam1, float uoz1, float voz1, float zr1, const Vector& n1,
								float x2f, float y2f, float z2, const Vertex& cam2, float uoz2, float voz2, float zr2, const Vector& n2,
								float x3f, float y3f, float z3, const Vertex& cam3, float uoz3, float voz3, float zr3, const Vector& n3,
								unsigned int textureCount, const Image* textures, std::vector<Light*>& lights)
	{
		if (_pixelBuffer)
		{
			const int y1 = (int)(16.0f * y1f + 0.5f);
			const int y2 = (int)(16.0f * y2f + 0.5f);
			const int y3 = (int)(16.0f * y3f + 0.5f);

			const int x1 = (int)(16.0f * x1f + 0.5f);
			const int x2 = (int)(16.0f * x2f + 0.5f);
			const int x3 = (int)(16.0f * x3f + 0.5f);

			// Precalculate half-space function coefficients (so they can be calculated incrementally in the loop)
			const int dy1 = x1 - x2;
			const int dy2 = x2 - x3;
			const int dy3 = x3 - x1;

			const int dx1 = y1 - y2;
			const int dx2 = y2 - y3;
			const int dx3 = y3 - y1;
			
			const int fdx1 = dx1 << 4;
			const int fdx2 = dx2 << 4;
			const int fdx3 = dx3 << 4;
			
			const int fdy1 = dy1 << 4;
			const int fdy2 = dy2 << 4;
			const int fdy3 = dy3 << 4;

			// Work out min and max X and Y
			int minX = (min((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int maxX = (max((int)x1, (int)x2, (int)x3) + 0xF) >> 4;
			int minY = (min((int)y1, (int)y2, (int)y3) + 0xF) >> 4;
			int maxY = (max((int)y1, (int)y2, (int)y3) + 0xF) >> 4;

			// Make sure it's not outside of the screen
			if (minX < 0)
				minX = 0;
			if (minY < 0)
				minY = 0;
			if (maxX >= _width)
				maxX = _width - 1;
			if (maxY >= _height)
				maxY = _height - 1;
			
			// Calculate half-space initial values
			int check1 = (int)((dy1 * (minY << 4)) - (dy1 * y1) - (dx1 * (minX << 4)) + (dx1 * x1));
			int check2 = (int)((dy2 * (minY << 4)) - (dy2 * y2) - (dx2 * (minX << 4)) + (dx2 * x2));
			int check3 = (int)((dy3 * (minY << 4)) - (dy3 * y3) - (dx3 * (minX << 4)) + (dx3 * x3));

			// Extend values if required for fill convention purposes
			if (dx1 < 0 || (dx1 == 0 && dy1 > 0))
				check1++;
			if (dx2 < 0 || (dx2 == 0 && dy2 > 0))
				check2++;
			if (dx3 < 0 || (dx3 == 0 && dy3 > 0)) 
				check3++;

			// Calculate initial normals
			Vector dxN;
			Vector dyN;
			Vector initialN = calculateInterpolants((float)minX, (float)minY, x1f, y1f, n1,
													x2f, y2f, n2, x3f, y3f, n3, &dxN, &dyN);

			// The same for z-coordinate for z-buffering
			float dxZ;
			float dyZ;
			float initialZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, z1,
													x2f, y2f, z2, x3f, y3f, z3, &dxZ, &dyZ);

			// Also camera space z needs to be interpolated for perspective correct textures/lighting
			// Camera-space z is too imprecise for z-buffering :<
			Vertex dxCam;
			Vertex dyCam;
			Vertex initialCam = calculateInterpolants((float)minX, (float)minY, x1f, y1f, cam1,
													x2f, y2f, cam2, x3f, y3f, cam3, &dxCam, &dyCam);

			// U / Z interpolation
			float dxUOZ;
			float dyUOZ;
			float initialUOZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, uoz1,
												x2f, y2f, uoz2, x3f, y3f, uoz3, &dxUOZ, &dyUOZ);

			// V / Z interpolation
			float dxVOZ;
			float dyVOZ;
			float initialVOZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, voz1,
													x2f, y2f, voz2, x3f, y3f, voz3, &dxVOZ, &dyVOZ);

			// 1 / Z interpolation
			float dxOOZ;
			float dyOOZ;
			float initialOOZ = calculateInterpolants((float)minX, (float)minY, x1f, y1f, zr1,
													x2f, y2f, zr2, x3f, y3f, zr3, &dxOOZ, &dyOOZ);
						
#ifdef SSE
			// Load interpolants __m128s for fast floating point addition
			__m128 normalf = { initialN.getX(), initialN.getY(), initialN.getZ(), initialZ };
			__m128 dxnf = { dxN.getX(), dxN.getY(), dxN.getZ(), dxZ };
			__m128 dynf = { dyN.getX(), dyN.getY(), dyN.getZ(), dyZ };

			// Load UV interpolants too
			__m128 initialUVf = { initialUOZ, initialVOZ, initialOOZ, 0 };
			__m128 dxUVf = { dxUOZ, dxVOZ, dxOOZ, 0 };
			__m128 dyUVf = { dyUOZ, dyVOZ, dyOOZ, 0 };

			// Temporary vector to store normal for calculations
			Vector currentNormal;
#endif

			// Calculate address offset of initial position in buffer
			unsigned int* buffer = (unsigned int*)_pixelBuffer + minY * _width;
			float* depthBuffer = _depthBuffer + minY * _width;

			// Temporary vertex to store position for calculations
			Vertex currentPosition;

			for (int y = minY; y < maxY; y++)
			{
				// Create temporary copies of our incremental values because they'll be needed in the next iteration
				int check1Temp = check1;
				int check2Temp = check2;
				int check3Temp = check3;
				
				float z;
				Vertex camSpacePos = initialCam;
#ifdef SSE
				__m128 currentNormalf = normalf;
				__m128 UVf = initialUVf;
#else
				z = initialZ;
				Vector currentNormal = initialN;

				float UOZ = initialUOZ;
				float VOZ = initialVOZ;
				float OOZ = initialOOZ;
#endif

				for (int x = minX; x < maxX; x++)
				{
					if (check1Temp > 0 &&
						check2Temp > 0 &&
						check3Temp > 0)
					{
#ifdef SSE
						z = currentNormalf.m128_f32[3];
#endif
						// If on top of screen
						if (depthBuffer[x] >= z)
						{						
							depthBuffer[x] = z;
							
#ifdef SSE
							currentNormal.setX(currentNormalf.m128_f32[0]);
							currentNormal.setY(currentNormalf.m128_f32[1]);
							currentNormal.setZ(currentNormalf.m128_f32[2]);
#endif

							Colour currentC = md2::MD2_Model::calculateLights(camSpacePos, currentNormal, lights);

							int i1;
							int i2;
							int i3;

							int tU;
							int tV;

#ifdef SSE
							__m128 currentColourf = { currentC._b, currentC._g, currentC._r, 0 };
#endif

#ifdef SSE2
							// Calculate perspective-correct texture coordinates
							float f;

							f = UVf.m128_f32[0] / UVf.m128_f32[2];
							__asm fld f
							__asm fistp tU
							
							f = UVf.m128_f32[1] / UVf.m128_f32[2];
							__asm fld f
							__asm fistp tV
							
							if (tU < 0)
								tU = 0;
							if (tV < 0)
								tV = 0;
							if (tU >= textures[0].getWidth())
								tU = textures[0].getWidth() - 1;
							if (tV >= textures[0].getHeight())
								tV = textures[0].getHeight() - 1;

							// Get texture colour at current (U,V) and multiply with light colour
							unsigned char* texture = (unsigned char*)&textures[0].getData()[(tV * textures[0].getWidth()) + tU];
							__m128 textureColour = { texture[0], texture[1], texture[2], 0 };

							// Scale it to the range 0 .. 1.0f
							static const __m128 twofivefive = { 255.0f, 255.0f, 255.0f, 255.0f };
							textureColour = _mm_div_ps(textureColour, twofivefive);

							// Multiply with light colour
							__m128 colour = _mm_mul_ps(currentColourf, textureColour);

							// Multiply by 255.0f
							colour = _mm_mul_ps(colour, twofivefive);

							// Clamp to 255.0f
							colour = _mm_min_ps(colour, twofivefive);

							// Convert our floating point colours to 32-bit integers
							__m128i intsOut = _mm_cvtps_epi32(colour);

							// Get the low-order bytes of each component
							i1 = intsOut.m128i_i8[0];
							i2 = intsOut.m128i_i8[4];
							i3 = intsOut.m128i_i8[8];

#else
#ifdef SSE
							// Calculate perspective-correct texture coordinates
							float f;

							f = UVf.m128_f32[0] / UVf.m128_f32[2];
							__asm fld f
							__asm fistp tU
							
							f = UVf.m128_f32[1] / UVf.m128_f32[2];
							__asm fld f
							__asm fistp tV
							
							if (tU < 0)
								tU = 0;
							if (tV < 0)
								tV = 0;
							if (tU >= textures[0].getWidth())
								tU = textures[0].getWidth() - 1;
							if (tV >= textures[0].getHeight())
								tV = textures[0].getHeight() - 1;

							// Get texture colour at current (U,V) and multiply with light colour
							unsigned char* texture = (unsigned char*)&textures[0].getData()[(tV * textures[0].getWidth()) + tU];
							__m128 textureColour = { texture[0], texture[1], texture[2], 0 };

							// Scale it to the range 0 .. 1.0f
							static const __m128 twofivefive = { 255.0f, 255.0f, 255.0f, 255.0f };
							textureColour = _mm_div_ps(textureColour, twofivefive);

							// Blend them by way of multiply
							__m128 colour = _mm_mul_ps(currentColourf, textureColour);

							// Multiply by 255.0f
							colour = _mm_mul_ps(colour, twofivefive);

							// Clamp to 255.0f
							colour = _mm_min_ps(colour, twofivefive);

							f = colour.m128_f32[0];						// Prepare float for conversion
							__asm fld f									// Push it onto the FLU stack
							__asm fistp i1								// Magic it to an int

							f = colour.m128_f32[1];
							__asm fld f
							__asm fistp i2

							f = colour.m128_f32[2];
							__asm fld f
							__asm fistp i3
#else
							Colour colour = currentC;

							// Calculate perspective-correct U and V
							tU = (int)(UOZ / OOZ);
							tV = (int)(VOZ / OOZ);
							
							if (tU < 0)
								tU = 0;
							if (tV < 0)
								tV = 0;
							if (tU >= textures[0].getWidth())
								tU = textures[0].getWidth() - 1;
							if (tV >= textures[0].getHeight())
								tV = textures[0].getHeight() - 1;

							unsigned char* texture = (unsigned char*)&textures[0].getData()[(tV * textures[0].getWidth()) + tU];
							Colour textureColour(texture[2], texture[1], texture[0]);
							textureColour /= 255.0f;

							colour *= textureColour;
							colour *= 255.0f;
							colour.clamp(250.0f);

							i1 = (int)colour._b;
							i2 = (int)colour._g;
							i3 = (int)colour._r;
#endif
#endif

							((unsigned char*)&buffer[x])[0] = i1;
							((unsigned char*)&buffer[x])[1] = i2;
							((unsigned char*)&buffer[x])[2] = i3;
						}
					}

					// Increment values in x
					check1Temp -= fdx1;
					check2Temp -= fdx2;
					check3Temp -= fdx3;

					camSpacePos += dxCam;

#ifdef SSE
					currentNormalf = _mm_add_ps(currentNormalf, dxnf);
					UVf = _mm_add_ps(UVf, dxUVf);
#else
					currentNormal += dxN;
					z += dxZ;

					// Interpolate u/z, v/z and 1/z
					UOZ += dxUOZ;
					VOZ += dxVOZ;
					OOZ += dxOOZ;
#endif
				}

				buffer += _width;
				depthBuffer += _width;

				// Increment values in y

				check1 += fdy1;
				check2 += fdy2;
				check3 += fdy3;

				initialCam += dyCam;

#ifdef SSE
				normalf = _mm_add_ps(normalf, dynf);
				initialUVf = _mm_add_ps(initialUVf, dyUVf);
#else
				initialN += dyN;
				initialZ += dyZ;

				initialUOZ += dyUOZ;
				initialVOZ += dyVOZ;
				initialOOZ += dyOOZ;
#endif
			}
		}
	}
}