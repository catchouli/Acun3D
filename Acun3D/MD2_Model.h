#ifndef __MD2_Model_H__
#define __MD2_Model_H__

#include <fstream>
#include <vector>

#include "MD2_Structures.h"
#include "Vertex.h"
#include "Vector.h"
#include "Triangle.h"
#include "UV.h"
#include "Image.h"
#include "Colour.h"
#include "Light.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "AmbientLight.h"
#include "Spotlight.h"

namespace a3d
{
	namespace md2
	{
		class MD2_Model
		{
		public:
			MD2_Model();
			~MD2_Model();

			bool loadModel(const char* filename);

			static Colour calculateLights(a3d::Vector& position, a3d::Vector& normal, std::vector<Light*>& lights);

			void setTexture(const char* filename);

			void processVertices(a3d::Vertex* vertexBuffer, long time);
			const a3d::Triangle* getFaces() const;

			void setAnimation(int fps = -1, int start = -1, int end = -1);
			void setScale(float scale);

			const UV* getUVs() const;
			const Image* getTextures() const;
			
			int getTextureCount() const;
			int getVertexCount() const;
			int getTriangleCount() const;
			int getCurrentFrame() const;

			static a3d::Vector standardNormals[];

		private:
			void animate(long time);
			void interpolate(a3d::Vertex* vertexBuffer) const;
			bool loadTexture(const char* filename);

			static Colour calculateLight(a3d::Vector& position, a3d::Vector& normal, Light& light);
			
			int _skinCount;
			int _frameCount;
			int _vertexCount;
			int _triangleCount;

			a3d::Triangle* _triangles;
			a3d::Vertex* _vertices;

			UV* _uvs;
			Image* _textures;

			unsigned int _textureId;
			AnimationState _animation;
			float _scale;
		};
	}
}

#endif