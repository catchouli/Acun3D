#include "MD2_Model.h"

namespace a3d
{
	namespace md2
	{
		MD2_Model::MD2_Model()
		{
			_vertices = 0;
			_triangles = 0;

			_uvs = 0;
			_textures = 0;

			_frameCount = 0;
			_vertexCount = 0;

			_textureId = 0;
			_scale = 1.0f;

			setAnimation();
		}

		MD2_Model::~MD2_Model()
		{
			if (_vertices)
				delete[] _vertices;

			if (_triangles)
				delete[] _triangles;

			if (_uvs)
				delete[] _uvs;
			
			if (_textures)
				delete[] _textures;
		}

		bool MD2_Model::loadModel(const char* filename)
		{
			std::ifstream in;
			MD2_Header header;
			char* buffer;

			// Attempt to open model file
			in.open(filename, std::ios::in | std::ios::binary);

			if (!in)
				return false;

			// Load header from file
			in.read((char*)&header, sizeof(MD2_Header));

			// Check this is actually an MD2 file
			if (header.ident == MD2_IDENT && header.version == 8)
			{
				// Initialise members
				_skinCount = header.skinCount;
				_frameCount = header.frameCount;
				_vertexCount = header.vertexCount;
				_triangleCount = header.triangleCount;
				_animation.lastFrame = header.frameCount - 1;

				// Allocate memory for model data
				_vertices = new a3d::Vertex[_vertexCount * _frameCount];
				buffer = new char[_frameCount * header.frameSize];

				// Read all frames
				in.seekg(header.frameOffset);
				in.read((char*)buffer, _frameCount * header.frameSize);

				// Read texture names
				TextureName* textureNames = new TextureName[_skinCount];
				in.seekg(header.skinOffset);
				in.read((char*)textureNames, _skinCount * sizeof(TextureName));

				// Load textures - check if _skinCount > 0 or new returns a pointer to nothing
				if (_skinCount > 0)
				{
					_textures = new Image[_skinCount];
					for (int i = 0, j = 0; i < _skinCount; ++i, ++j)
					{
						bool success = _textures[i].load(textureNames[i]);

						if (!success)
						{
							_skinCount--;
							j--;
						}
					}
				}

				// Load frames
				for (int i = 0; i < _frameCount; ++i)
				{
					Frame* frame = (Frame*)&buffer[header.frameSize * i];
					a3d::Vertex* vertex = &_vertices[_vertexCount * i];

					// Load vertices
					for (int j = 0; j < _vertexCount; ++j)
					{
						vertex[j](0, 0) = (frame->vertices[j].position[0] * frame->scale[0]) + frame->translate[0];
						vertex[j](1, 0) = (frame->vertices[j].position[1] * frame->scale[1]) + frame->translate[1];
						vertex[j](2, 0) = (frame->vertices[j].position[2] * frame->scale[2]) + frame->translate[2];
						vertex[j](3, 0) = 1;

						vertex[j].setNormal(-standardNormals[frame->vertices[j].normalIndex]);
					}
				}

				// Read texture coordinates
				TextureCoords* textureCoords = new TextureCoords[header.texCoordCount];
				in.seekg(header.texCoordOffset);
				in.read((char*)textureCoords, header.texCoordCount * sizeof(TextureCoords));

				// Store them to the model
				if (header.texCoordCount > 0)
				{
					_uvs = new UV[header.texCoordCount];
					for (int i = 0; i < header.texCoordCount; ++i)
					{
						_uvs[i].U = textureCoords[i].s;
						_uvs[i].V = textureCoords[i].t;
					}
				}

				// Read triangles
				_triangles = new a3d::Triangle[_triangleCount];
				Triangle* tris = new Triangle[header.triangleCount];
				in.seekg(header.triangleOffset);
				in.read((char*)tris, header.triangleCount * sizeof(Triangle));

				// Load them
				for (int i = 0; i < _triangleCount; ++i)
				{
					// Vertices
					_triangles[i].A = tris[i].vertex[0];
					_triangles[i].B = tris[i].vertex[1];
					_triangles[i].C = tris[i].vertex[2];

					// Store texture coordinate indicies
					_triangles[i].AT = tris[i].texCoordIndices[0];
					_triangles[i].BT = tris[i].texCoordIndices[1];
					_triangles[i].CT = tris[i].texCoordIndices[2];

					// Calculate normal
					_triangles[i].normals = new a3d::Vector[_frameCount];
					for (int j = 0; j < _frameCount; ++j)
					{
						// Pointer to current frame
						a3d::Vertex* frame = &_vertices[j * _vertexCount];
						
						// References to current face's vertex
						a3d::Vertex& A = frame[_triangles[i].A];
						a3d::Vertex& B = frame[_triangles[i].B];
						a3d::Vertex& C = frame[_triangles[i].C];

						// Calculate surface normal
						a3d::Vector BA = A - B;
						a3d::Vector AC = A - C;

						// Store it in triangle
						_triangles[i].normals[j] = BA.cross(AC);
					}
				}

				delete[] buffer;
				delete[] textureCoords;
				delete[] tris;
				delete[] textureNames;
			}
			else
			{
				// Fail if incorrect
				in.close();
				return false;				
			}		

			in.close();
			return true;
		}
		
		Colour MD2_Model::calculateLights(a3d::Vector& position, a3d::Vector& normal, std::vector<Light*>& lights)
		{
			Colour colour(0, 0, 0);

			for (unsigned int i = 0; i < lights.size(); ++i)
			{
				colour += calculateLight(position, normal, *lights[i]);
			}

			colour.clamp(1.0f);

			return colour;
		}

		Colour MD2_Model::calculateLight(a3d::Vector& position, a3d::Vector& normal, Light& light)
		{
			static const int specularExponent = 32;
			static const float specularCoefficient = 1.0f;
			static const float diffuseCoefficient = 0.7f;

			LightType type = light.getType();
				a3d::Vector lightDirection(0, 0, 0);

			float dot;
			float spotFactor;

			if (type == LightTypes::AMBIENT)
			{
				AmbientLight& ambient = (AmbientLight&)light;

				Colour colour = light.getColour();
				colour *= ambient.getIntesity();

				return colour;
			}
			else
			{
				Colour lightColour = light.getColour();

				float total = 0;

				if (type == LightTypes::SPOT)
				{
					Spotlight& spotlight = (Spotlight&)light;

					lightDirection = position - spotlight.getPosition();
					lightDirection.normalise();

					dot = lightDirection.dot(spotlight.getDirection());

					if (dot >= cos(spotlight.getFOV()))
						spotFactor = pow(dot, spotlight.getExponent());
					else
						spotFactor = 0;

					if (spotFactor < 0)
						spotFactor = 0;
				}
				else
				{
					spotFactor = 1;
				}

				if (type == LightTypes::POINT)
				{
					lightDirection = position - ((PointLight&)light).getPosition();
					lightDirection.normalise();
				}
				else if (type == LightTypes::DIRECTIONAL)
				{
					lightDirection = ((DirectionalLight&)light).getDirection();
					lightDirection.normalise();
				}


				a3d::Vector cameraDirection = position;
				cameraDirection.normalise();

				float cosLightNormal = lightDirection.dot(normal);

				float diffuse = (cosLightNormal + 1.0f) / 2.0f;

				a3d::Vector reflected = normal * (2.0f * cosLightNormal);
				reflected = reflected - lightDirection;

				float specular = reflected.dot(cameraDirection);
				if (specular < 0)
					specular = 0;
				else
					specular = (float)pow(specular, specularExponent);

				total = (specular * specularCoefficient) + (diffuse * diffuseCoefficient);
		
				// Take account of if it's a spot light
				total *= spotFactor;

				// Take account of the colour of the light
				lightColour *= total;

				return lightColour;
			}
		}

		void MD2_Model::setTexture(const char* filename)
		{
			if (_skinCount == 0)
			{
				_skinCount = 1;

				if (_textures != 0)
					delete _textures;

				_textures = new Image[1];
				bool success = _textures->load(filename);

				if (!success)
					_skinCount--;
			}
		}

		void MD2_Model::processVertices(a3d::Vertex* vertexBuffer, long time)
		{
			// Update animation based on time
			animate(time);

			// Interpolate vertex positions
			interpolate(vertexBuffer);
		}

		const a3d::Triangle* MD2_Model::getFaces() const
		{
			return _triangles;
		}

		void MD2_Model::setAnimation(int fps, int start, int end)
		{
			_animation.fps = fps;

			if (fps >= 0 && _frameCount > start)
			{
				if (start < 0)
				{
					_animation.firstFrame = 0;
					_animation.lastFrame = _frameCount - 1;
				}
				else
				{
					_animation.firstFrame = start;
					if (end < 0)
						end = _frameCount - 1;
					else
						_animation.lastFrame = end;
				}
				
				_animation.curFrame = _animation.firstFrame;
				if (start + 1 < _frameCount)
					_animation.nextFrame = _animation.firstFrame + 1;
				else
					_animation.nextFrame = _animation.firstFrame;
			}
		}

		void MD2_Model::animate(long time)
		{
			_animation.curTime = time;

			if (_animation.fps > 0)
			{
				if ((_animation.curTime - _animation.oldTime) > (1000 / _animation.fps))
				{
					_animation.curFrame = _animation.nextFrame;
					_animation.nextFrame++;

					if (_animation.nextFrame > _animation.lastFrame)
						_animation.nextFrame = _animation.firstFrame;

					_animation.oldTime = _animation.curTime;
				}
			
				if (_animation.curFrame > _frameCount - 1)
					_animation.curFrame = 0;

				if (_animation.nextFrame > _frameCount - 1)
					_animation.nextFrame = 0;

				_animation.curInterpolation = _animation.fps * ((_animation.curTime - _animation.oldTime) / 1000.0f);
			}
		}

		void MD2_Model::interpolate(a3d::Vertex* vertexBuffer) const
		{
			a3d::Vertex* currentFrame = &_vertices[_vertexCount * _animation.curFrame];
			a3d::Vertex* nextFrame = &_vertices[_vertexCount * _animation.nextFrame];

			for (int i = 0; i < _vertexCount; ++i)
			{
				a3d::Vertex& currentVertex = currentFrame[i];
				a3d::Vertex& nextVertex = nextFrame[i];
				
				vertexBuffer[i] = (currentVertex + ((nextVertex - currentVertex) * _animation.curInterpolation)) * _scale;
				vertexBuffer[i](3, 0) = 1;
				
				a3d::Vector currentNormal = currentVertex.getNormal();
				a3d::Vector nextNormal = nextVertex.getNormal();

				a3d::Vector interpolated = (currentNormal + ((nextNormal - currentNormal) * _animation.curInterpolation)) * _scale;

				vertexBuffer[i].setNormal(interpolated);
			}
		}

		const UV* MD2_Model::getUVs() const
		{
			return _uvs;
		}

		const Image* MD2_Model::getTextures() const
		{
			return _textures;
		}
			
		int MD2_Model::getTextureCount() const
		{
			return _skinCount;
		}
		
		int MD2_Model::getVertexCount() const
		{
			return _vertexCount;
		}

		int MD2_Model::getTriangleCount() const
		{
			return _triangleCount;
		}

		int MD2_Model::getCurrentFrame() const
		{
			return _animation.curFrame;
		}

		a3d::Vector MD2_Model::standardNormals[] =
		{
			#include "MD2_Normals.h"
		};
	}
}