#include "Renderer.h"

namespace a3d
{
	Renderer::Renderer(float nearView, float farView)
		: _matrixStack(&_world), _fullBright(1.0f), _nearView(nearView), _farView(farView)
	{
		_rasteriser = 0;
		_width = 0;
		_height = 0;

		// Default rendering mode
		_materialType = MaterialTypes::TEXTURED;
		_shadingType = ShadingTypes::SMOOTH;
		_cullingType = CullingTypes::BACK;

		_full.push_back(&_fullBright);
	}

	Renderer::Renderer(Pixel* pixelBuffer, int width, int height, float nearView, float farView)
		: _matrixStack(&_world), _fullBright(0), _nearView(nearView), _farView(farView)
	{
		_rasteriser = new Rasteriser(pixelBuffer, width, height);
		_width = width;
		_height = height;
		
		// Default rendering mode
		_materialType = MaterialTypes::TEXTURED;
		_shadingType = ShadingTypes::SMOOTH;
		_cullingType = CullingTypes::BACK;

		_full.push_back(&_fullBright);
	}

	Renderer::~Renderer()
	{
		delete _rasteriser;
	}

	bool Renderer::draw(md2::MD2_Model& model, long time)
	{
		// Store current matrix mode
		MatrixMode mode = _matrixMode;

		// Get current view matrix
		setMatrixMode(MatrixModes::VIEW);
		Matrix4f view = getMatrix();

		// Push it onto the modelview stack
		setMatrixMode(MatrixModes::WORLD);

		// Do full-model near and far plane culling
		if (_nearView < _farView)
		{
			Matrix4f m = getMatrix();
			
			float z = m(2, 3);
			float viewz = view(2, 3);

			if (z < -viewz-_farView)
				return false;
			if (z > -viewz-_nearView)
				return true;
		}

		// Transform all lights to the view
		// Make a copy of the old pointers
		std::vector<Light*> lights = _lights;

		// Create new lights that have been transformed
		_lights.clear();
		for (unsigned int i = 0; i < lights.size(); ++i)
		{
			Light* light = 0;
			PointLight* s;

			Vertex4f position;

			switch (lights[i]->getType())
			{
			case LightTypes::AMBIENT:
				light = new AmbientLight(*(AmbientLight*)lights[i]);
				break;
			case LightTypes::DIRECTIONAL:
				light = new DirectionalLight(*(DirectionalLight*)lights[i]);
				break;
			case LightTypes::POINT:
				{
					position = ((PointLight*)lights[i])->getPosition();
					position(3, 0) = 1;
					position = view * position;
					light = new PointLight(Vector(position(0, 0), position(1, 0), position(2, 0)), lights[i]->getColour());
					s = (PointLight*)light;
				}
				break;
			case LightTypes::SPOT:
				{
					position = ((PointLight*)lights[i])->getPosition();
					position(3, 0) = 1;
					position = view * position;
					light = new Spotlight(view * ((Spotlight*)lights[i])->getPosition(), ((Spotlight*)lights[i])->getDirection(), 
						lights[i]->getColour(), ((Spotlight*)lights[i])->getFOV(), ((Spotlight*)lights[i])->getExponent());
				}
				break;
			}

			if (light != 0)
				_lights.push_back(light);
		}

		pushMatrix();
			transform(view);

			MaterialType old = _materialType;
			if (_materialType == MaterialTypes::TEXTURED && model.getTextureCount() <= 0)
				_materialType = MaterialTypes::SOLID;

			// Draw model based on renderer state
			switch (_materialType)
			{
			case MaterialTypes::WIREFRAME:
				{
					drawWireFrame(model, time);
				}
				break;

			case MaterialTypes::SOLID:
				{
					if (_shadingType == ShadingTypes::SMOOTH)
						drawSolidSmooth(model, time);
					else if (_shadingType == ShadingTypes::PHONG)
						drawSolidPhong(model, time);
					else
						drawSolidFlat(model, time);
				}
				break;

			case MaterialTypes::TEXTURED:
				{
					if (_shadingType == ShadingTypes::SMOOTH)
						drawSolidSmoothTextured(model, time);
					else if (_shadingType == ShadingTypes::PHONG)
						drawSolidPhongTextured(model, time);
					else
						drawSolidFlatTextured(model, time);
				}
				break;
			}
		popMatrix();

		_materialType = old;

		// Clean up temp lighting
		for (unsigned int i = 0; i < _lights.size(); ++i)
		{
			delete[] _lights[i];
		}

		_lights.clear();
		_lights = lights;

		setMatrixMode(mode);

		return true;
	}

	void Renderer::drawWireFrame(md2::MD2_Model& model, long time)
	{
		int vertexCount = model.getVertexCount();
		int triangleCount = model.getTriangleCount();

		// Get reference to faces
		const a3d::Triangle* triangles = model.getFaces();

		// Create vertex buffer
		Vertex* vertexBuffer = new Vertex[vertexCount];
		Vertex* cam = new Vertex[vertexCount];
		Vertex* screen = new Vertex[vertexCount];

		// Process vertices
		model.processVertices(vertexBuffer, time);

		// Transform to world space
		for (int i = 0; i < vertexCount; ++i)
		{
			// Do model view transformation
			cam[i] = operator*(_world.top(), vertexBuffer[i]);

			screen[i] = operator*(_projection.top(), cam[i]);
			
			screen[i](0, 0) /= screen[i](3, 0);
			screen[i](1, 0) /= screen[i](3, 0);
			screen[i](2, 0) /= screen[i](3, 0);
		}

		int frame = model.getCurrentFrame();
		for (int i = 0; i < triangleCount; ++i)
		{
			Vertex& v1 = screen[triangles[i].A];
			Vertex& v2 = screen[triangles[i].B];
			Vertex& v3 = screen[triangles[i].C];
			
			// Cull polygon before the near plane
			if (v1.getZ() < 0 || v2.getZ() < 0 || v3.getZ() < 0 ||
			// or beyond the far plane
				v1.getZ() > 1 || v2.getZ() > 1 || v3.getZ() > 1 ||
			// or if all three vertices are entirely outside of the view
				((v1.getX() < -1 || v1.getX() > 1 || v1.getY() < -1 || v1.getX() > 1) &&
				(v2.getX() < -1 || v2.getX() > 1 || v2.getY() < -1 || v2.getX() > 1) &&
				(v3.getX() < -1 || v3.getX() > 1 || v3.getY() < -1 || v3.getX() > 1)))
				continue;

			Vector normal = _world.top() * triangles[i].normals[frame];
			
			Vertex& v1cam = cam[triangles[i].A];
			Vector cameraDirection = v1cam;

			float cos = normal.dot(cameraDirection);

			if (cos < 0 && _cullingType == CullingTypes::BACK
				|| cos > 0 && _cullingType == CullingTypes::FRONT)
				continue;

			int x1 = int(v1(0, 0) * _width + _width/2.0f);
			int y1 = int(v1(1, 0) * _height + _height/2.0f);
			int x2 = int(v2(0, 0) * _width + _width/2.0f);
			int y2 = int(v2(1, 0) * _height + _height/2.0f);
			int x3 = int(v3(0, 0) * _width + _width/2.0f);
			int y3 = int(v3(1, 0) * _height + _height/2.0f);
			
			_rasteriser->drawLine(x1, y1, x2, y2, 0x00FF00FF);
			_rasteriser->drawLine(x2, y2, x3, y3, 0x00FF00FF);
			_rasteriser->drawLine(x3, y3, x1, y1, 0x00FF00FF);
		}

		// Clean up memory
		delete[] vertexBuffer;
		delete[] cam;
		delete[] screen;
	}

	void Renderer::drawSolidFlat(md2::MD2_Model& model, long time)
	{
		std::vector<Light*>& lights = _lights;

		if (_shadingType == ShadingTypes::NONE)
			lights = _full;

		// TODO: interpolate normals for flat shading animation
		int vertexCount = model.getVertexCount();
		int triangleCount = model.getTriangleCount();

		// Get reference to faces
		const a3d::Triangle* triangles = model.getFaces();

		// Create vertex buffer
		Vertex* vertexBuffer = new Vertex[vertexCount];
		Vertex* cam = new Vertex[vertexCount];
		Vertex* screen = new Vertex[vertexCount];

		// Process vertices
		model.processVertices(vertexBuffer, time);

		// Transform to world space
		for (int i = 0; i < vertexCount; ++i)
		{
			// Do model view transformation
			cam[i] = operator*(_world.top(), vertexBuffer[i]);

			screen[i] = operator*(_projection.top(), cam[i]);
			
			screen[i](0, 0) /= screen[i](3, 0);
			screen[i](1, 0) /= screen[i](3, 0);
			screen[i](2, 0) /= screen[i](3, 0);
		}

		int frame = model.getCurrentFrame();
		for (int i = 0; i < triangleCount; ++i)
		{
			Vertex& v1 = screen[triangles[i].A];
			Vertex& v2 = screen[triangles[i].B];
			Vertex& v3 = screen[triangles[i].C];

			// Cull polygon before the near plane
			if (v1.getZ() < 0 || v2.getZ() < 0 || v3.getZ() < 0 ||
			// or beyond the far plane
				v1.getZ() > 1 || v2.getZ() > 1 || v3.getZ() > 1 ||
			// or if all three vertices are entirely outside of the view
				((v1.getX() < -1 || v1.getX() > 1 || v1.getY() < -1 || v1.getX() > 1) &&
				(v2.getX() < -1 || v2.getX() > 1 || v2.getY() < -1 || v2.getX() > 1) &&
				(v3.getX() < -1 || v3.getX() > 1 || v3.getY() < -1 || v3.getX() > 1)))
				continue;

			// Calculate camera-space normal
			Vector normal = _world.top() * triangles[i].normals[frame];
			
			// Get vertex to approximate polygon position
			Vertex& v = cam[triangles[i].A];

			float cos = normal.dot(v);

			if (cos < 0)
				continue;

			normal.normalise();

			float x1 = v1(0, 0) * _width + _width/2.0f;
			float y1 = v1(1, 0) * _height + _height/2.0f;
			float z1 = v1(3, 0);
			float x2 = v2(0, 0) * _width + _width/2.0f;
			float y2 = v2(1, 0) * _height + _height/2.0f;
			float z2 = v2(3, 0);
			float x3 = v3(0, 0) * _width + _width/2.0f;
			float y3 = v3(1, 0) * _height + _height/2.0f;
			float z3 = v3(3, 0);

			Colour colour(0, 0, 0);

			colour = model.calculateLights(v, normal, lights);

			colour.clamp(255.0f);
			
			_rasteriser->drawTriangle(x1, y1, z1, colour,
									x2, y2, z2, colour,
									x3, y3, z3, colour);
		}

		// Clean up memory
		delete[] vertexBuffer;
		delete[] cam;
		delete[] screen;
	}

	void Renderer::drawSolidFlatTextured(md2::MD2_Model& model, long time)
	{
		std::vector<Light*>& lights = _lights;

		if (_shadingType == ShadingTypes::NONE)
			lights = _full;

		// TODO: interpolate normals for flat shading animation
		int vertexCount = model.getVertexCount();
		int triangleCount = model.getTriangleCount();

		// Get reference to faces
		const a3d::Triangle* triangles = model.getFaces();

		// Create vertex buffer
		Vertex* vertexBuffer = new Vertex[vertexCount];
		Vertex* cam = new Vertex[vertexCount];
		Vertex* screen = new Vertex[vertexCount];

		// Process vertices
		model.processVertices(vertexBuffer, time);

		// Transform to world space
		for (int i = 0; i < vertexCount; ++i)
		{
			// Do model view transformation
			cam[i] = operator*(_world.top(), vertexBuffer[i]);

			screen[i] = operator*(_projection.top(), cam[i]);
			
			screen[i](0, 0) /= screen[i](3, 0);
			screen[i](1, 0) /= screen[i](3, 0);
			screen[i](2, 0) /= screen[i](3, 0);
		}

		int frame = model.getCurrentFrame();
		for (int i = 0; i < triangleCount; ++i)
		{
			const Triangle& triangle = triangles[i];

			Vertex& v1 = screen[triangle.A];
			Vertex& v2 = screen[triangle.B];
			Vertex& v3 = screen[triangle.C];


			// Cull polygon before the near plane
			if (v1.getZ() < 0 || v2.getZ() < 0 || v3.getZ() < 0 ||
			// or beyond the far plane
				v1.getZ() > 1 || v2.getZ() > 1 || v3.getZ() > 1 ||
			// or if all three vertices are entirely outside of the view
				((v1.getX() < -1 || v1.getX() > 1 || v1.getY() < -1 || v1.getX() > 1) &&
				(v2.getX() < -1 || v2.getX() > 1 || v2.getY() < -1 || v2.getX() > 1) &&
				(v3.getX() < -1 || v3.getX() > 1 || v3.getY() < -1 || v3.getX() > 1)))
				continue;

			// Calculate camera-space normal
			Vector normal = _world.top() * triangles[i].normals[frame];
			
			// Get vertex to approximate polygon position
			Vertex& v = cam[triangles[i].A];

			float cos = normal.dot(v);

			if (cos < 0)
				continue;

			normal.normalise();

			float x1 = v1(0, 0) * _width + _width/2.0f;
			float y1 = v1(1, 0) * _height + _height/2.0f;
			float z1 = v1(3, 0);
			float x2 = v2(0, 0) * _width + _width/2.0f;
			float y2 = v2(1, 0) * _height + _height/2.0f;
			float z2 = v2(3, 0);
			float x3 = v3(0, 0) * _width + _width/2.0f;
			float y3 = v3(1, 0) * _height + _height/2.0f;
			float z3 = v3(3, 0);

			Colour colour = model.calculateLights(v, normal, lights);

			colour.clamp(255.0f);

			const UV* uvs = model.getUVs();

			// Get UV coordinates
			float AU = uvs[triangle.AT].U;
			float AV = uvs[triangle.AT].V;
			float BU = uvs[triangle.BT].U;

			float BV = uvs[triangle.BT].V;
			float CU = uvs[triangle.CT].U;
			float CV = uvs[triangle.CT].V;

			// Camera-space z coordinates
			float AZ = cam[triangle.A].getZ();
			float BZ = cam[triangle.B].getZ();
			float CZ = cam[triangle.C].getZ();
			
			// Pass in U/z, V/z and 1/z
			_rasteriser->drawTriangle(x1, y1, z1, AU / AZ, AV / AZ, 1 / AZ, colour,
									x2, y2, z2, BU / BZ, BV / BZ, 1 / BZ, colour,
									x3, y3, z3, CU / CZ, CV / CZ, 1 / CZ, colour,
									model.getTextureCount(), model.getTextures());
		}

		// Clean up memory
		delete[] vertexBuffer;
		delete[] cam;
		delete[] screen;
	}

	void Renderer::drawSolidSmooth(md2::MD2_Model& model, long time)
	{
		int vertexCount = model.getVertexCount();
		int triangleCount = model.getTriangleCount();

		// Get reference to faces
		const a3d::Triangle* triangles = model.getFaces();

		// Create vertex buffer
		Vertex* vertexBuffer = new Vertex[vertexCount];
		Vertex* cam = new Vertex[vertexCount];
		Vertex* screen = new Vertex[vertexCount];

		// Buffer for vertex normals
		Vector* normalBuffer = new Vertex[vertexCount];

		// Process vertices
		model.processVertices(vertexBuffer, time);

		// Transform to world space
		for (int i = 0; i < vertexCount; ++i)
		{
			// Do model view transformation
			cam[i] = operator*(_world.top(), vertexBuffer[i]);
			normalBuffer[i] = _world.top() * vertexBuffer[i].getNormal();

			screen[i] = operator*(_projection.top(), cam[i]);
			
			screen[i](0, 0) /= screen[i](3, 0);
			screen[i](1, 0) /= screen[i](3, 0);
			screen[i](2, 0) /= screen[i](3, 0);
		}

		int frame = model.getCurrentFrame();
		for (int i = 0; i < triangleCount; ++i)
		{
			const Triangle& triangle = triangles[i];

			Vertex& v1 = screen[triangle.A];
			Vertex& v2 = screen[triangle.B];
			Vertex& v3 = screen[triangle.C];

			// Cull polygon before the near plane
			if (v1.getZ() < 0 || v2.getZ() < 0 || v3.getZ() < 0 ||
			// or beyond the far plane
				v1.getZ() > 1 || v2.getZ() > 1 || v3.getZ() > 1 ||
			// or if all three vertices are entirely outside of the view
				((v1.getX() < -1 || v1.getX() > 1 || v1.getY() < -1 || v1.getX() > 1) &&
				(v2.getX() < -1 || v2.getX() > 1 || v2.getY() < -1 || v2.getX() > 1) &&
				(v3.getX() < -1 || v3.getX() > 1 || v3.getY() < -1 || v3.getX() > 1)))
				continue;

			Vector normal = _world.top() * triangle.normals[frame];
			
			Vertex& v1cam = cam[triangle.A];
			Vertex& v2cam = cam[triangle.B];
			Vertex& v3cam = cam[triangle.C];

			float cos = normal.dot(v1cam);

			if (cos < 0)
				continue;

			normal.normalise();
						
			Vector normalA = normalBuffer[triangle.A].getNormalised();
			Vector normalB = normalBuffer[triangle.B].getNormalised();
			Vector normalC = normalBuffer[triangle.C].getNormalised();

			// Calculate lighting for vertices
			Colour colour1 = model.calculateLights(v1cam, normalA, _lights);
			Colour colour2 = model.calculateLights(v2cam, normalB, _lights);
			Colour colour3 = model.calculateLights(v3cam, normalC, _lights);

			float x1 = v1(0, 0) * _width + _width/2.0f;
			float y1 = v1(1, 0) * _height + _height/2.0f;
			float z1 = v1(2, 0);
			float x2 = v2(0, 0) * _width + _width/2.0f;
			float y2 = v2(1, 0) * _height + _height/2.0f;
			float z2 = v2(2, 0);
			float x3 = v3(0, 0) * _width + _width/2.0f;
			float y3 = v3(1, 0) * _height + _height/2.0f;
			float z3 = v3(2, 0);
			
			_rasteriser->drawTriangle(x1, y1, z1, colour1,
									x2, y2, z2, colour2,
									x3, y3, z3, colour3);
		}

		// Clean up memory
		delete[] vertexBuffer;
		delete[] normalBuffer;
		delete[] cam;
		delete[] screen;
	}

	void Renderer::drawSolidSmoothTextured(md2::MD2_Model& model, long time)
	{
		int vertexCount = model.getVertexCount();
		int triangleCount = model.getTriangleCount();

		// Get reference to faces
		const a3d::Triangle* triangles = model.getFaces();

		// Create vertex buffer
		Vertex* vertexBuffer = new Vertex[vertexCount];
		Vertex* cam = new Vertex[vertexCount];
		Vertex* screen = new Vertex[vertexCount];

		// Buffer for vertex normals
		Vector* normalBuffer = new Vertex[vertexCount];

		// Process vertices
		model.processVertices(vertexBuffer, time);

		// Transform to world space
		for (int i = 0; i < vertexCount; ++i)
		{
			// Do model view transformation
			cam[i] = operator*(_world.top(), vertexBuffer[i]);
			normalBuffer[i] = _world.top() * vertexBuffer[i].getNormal();

			screen[i] = operator*(_projection.top(), cam[i]);
			
			screen[i](0, 0) /= screen[i](3, 0);
			screen[i](1, 0) /= screen[i](3, 0);
			screen[i](2, 0) /= screen[i](3, 0);
		}

		int frame = model.getCurrentFrame();
		for (int i = 0; i < triangleCount; ++i)
		{
			const Triangle& triangle = triangles[i];

			Vertex& v1 = screen[triangle.A];
			Vertex& v2 = screen[triangle.B];
			Vertex& v3 = screen[triangle.C];
			
			// Cull polygon before the near plane
			if (v1.getZ() < 0 || v2.getZ() < 0 || v3.getZ() < 0 ||
			// or beyond the far plane
				v1.getZ() > 1 || v2.getZ() > 1 || v3.getZ() > 1 ||
			// or if all three vertices are entirely outside of the view
				((v1.getX() < -1 || v1.getX() > 1 || v1.getY() < -1 || v1.getX() > 1) &&
				(v2.getX() < -1 || v2.getX() > 1 || v2.getY() < -1 || v2.getX() > 1) &&
				(v3.getX() < -1 || v3.getX() > 1 || v3.getY() < -1 || v3.getX() > 1)))
				continue;

			Vector normal = _world.top() * triangle.normals[frame];
			
			Vertex& v1cam = cam[triangle.A];
			Vertex& v2cam = cam[triangle.B];
			Vertex& v3cam = cam[triangle.C];

			float cos = normal.dot(v1cam);

			if (cos < 0)
				continue;

			normal.normalise();
						
			Vector normalA = normalBuffer[triangle.A].getNormalised();
			Vector normalB = normalBuffer[triangle.B].getNormalised();
			Vector normalC = normalBuffer[triangle.C].getNormalised();

			// Calculate lighting for vertices
			Colour colour1 = model.calculateLights(v1cam, normalA, _lights);
			Colour colour2 = model.calculateLights(v2cam, normalB, _lights);
			Colour colour3 = model.calculateLights(v3cam, normalC, _lights);

			float x1 = v1(0, 0) * _width + _width/2.0f;
			float y1 = v1(1, 0) * _height + _height/2.0f;
			float z1 = v1(2, 0);
			float x2 = v2(0, 0) * _width + _width/2.0f;
			float y2 = v2(1, 0) * _height + _height/2.0f;
			float z2 = v2(2, 0);
			float x3 = v3(0, 0) * _width + _width/2.0f;
			float y3 = v3(1, 0) * _height + _height/2.0f;
			float z3 = v3(2, 0);

			const UV* uvs = model.getUVs();

			// Get UV coordinates
			float AU = uvs[triangle.AT].U;
			float AV = uvs[triangle.AT].V;
			float BU = uvs[triangle.BT].U;

			float BV = uvs[triangle.BT].V;
			float CU = uvs[triangle.CT].U;
			float CV = uvs[triangle.CT].V;

			// Camera-space z coordinates
			float AZ = cam[triangle.A].getZ();
			float BZ = cam[triangle.B].getZ();
			float CZ = cam[triangle.C].getZ();
			
			// Pass in U/z, V/z and 1/z
			_rasteriser->drawTriangle(x1, y1, z1, AU / AZ, AV / AZ, 1 / AZ, colour1,
									x2, y2, z2, BU / BZ, BV / BZ, 1 / BZ, colour2,
									x3, y3, z3, CU / CZ, CV / CZ, 1 / CZ, colour3,
									model.getTextureCount(), model.getTextures());
		}

		// Clean up memory
		delete[] vertexBuffer;
		delete[] normalBuffer;
		delete[] cam;
		delete[] screen;
	}
	
	void Renderer::drawSolidPhong(md2::MD2_Model& model, long time)
	{
		int vertexCount = model.getVertexCount();
		int triangleCount = model.getTriangleCount();

		// Get reference to faces
		const a3d::Triangle* triangles = model.getFaces();

		// Create vertex buffer
		Vertex* vertexBuffer = new Vertex[vertexCount];
		Vertex* cam = new Vertex[vertexCount];
		Vertex* screen = new Vertex[vertexCount];

		// Buffer for vertex normals
		Vector* normalBuffer = new Vertex[vertexCount];

		// Process vertices
		model.processVertices(vertexBuffer, time);

		// Transform to world space
		for (int i = 0; i < vertexCount; ++i)
		{
			// Do model view transformation
			cam[i] = operator*(_world.top(), vertexBuffer[i]);
			normalBuffer[i] = _world.top() * vertexBuffer[i].getNormal();

			screen[i] = operator*(_projection.top(), cam[i]);
			
			screen[i](0, 0) /= screen[i](3, 0);
			screen[i](1, 0) /= screen[i](3, 0);
			screen[i](2, 0) /= screen[i](3, 0);
		}

		int frame = model.getCurrentFrame();
		for (int i = 0; i < triangleCount; ++i)
		{
			const Triangle& triangle = triangles[i];

			Vertex& v1 = screen[triangle.A];
			Vertex& v2 = screen[triangle.B];
			Vertex& v3 = screen[triangle.C];

			// Cull polygon before the near plane
			if (v1.getZ() < 0 || v2.getZ() < 0 || v3.getZ() < 0 ||
			// or beyond the far plane
				v1.getZ() > 1 || v2.getZ() > 1 || v3.getZ() > 1 ||
			// or if all three vertices are entirely outside of the view
				((v1.getX() < -1 || v1.getX() > 1 || v1.getY() < -1 || v1.getX() > 1) &&
				(v2.getX() < -1 || v2.getX() > 1 || v2.getY() < -1 || v2.getX() > 1) &&
				(v3.getX() < -1 || v3.getX() > 1 || v3.getY() < -1 || v3.getX() > 1)))
				continue;

			Vector normal = _world.top() * triangle.normals[frame];
			
			Vertex& v1cam = cam[triangle.A];
			Vertex& v2cam = cam[triangle.B];
			Vertex& v3cam = cam[triangle.C];

			float cos = normal.dot(v1cam);

			if (cos < 0)
				continue;

			normal.normalise();
			
			Vector normalA = normalBuffer[triangle.A].getNormalised();
			Vector normalB = normalBuffer[triangle.B].getNormalised();
			Vector normalC = normalBuffer[triangle.C].getNormalised();

			float x1 = v1(0, 0) * _width + _width/2.0f;
			float y1 = v1(1, 0) * _height + _height/2.0f;
			float z1 = v1(2, 0);
			float x2 = v2(0, 0) * _width + _width/2.0f;
			float y2 = v2(1, 0) * _height + _height/2.0f;
			float z2 = v2(2, 0);
			float x3 = v3(0, 0) * _width + _width/2.0f;
			float y3 = v3(1, 0) * _height + _height/2.0f;
			float z3 = v3(2, 0);
			
			_rasteriser->drawTriangle(x1, y1, z1, v1cam, normalA,
									x2, y2, z2, v2cam, normalB,
									x3, y3, z3, v3cam, normalC, _lights);
		}

		// Clean up memory
		delete[] vertexBuffer;
		delete[] normalBuffer;
		delete[] cam;
		delete[] screen;
	}

	void Renderer::drawSolidPhongTextured(md2::MD2_Model& model, long time)
	{
		int vertexCount = model.getVertexCount();
		int triangleCount = model.getTriangleCount();

		// Get reference to faces
		const a3d::Triangle* triangles = model.getFaces();

		// Create vertex buffer
		Vertex* vertexBuffer = new Vertex[vertexCount];
		Vertex* cam = new Vertex[vertexCount];
		Vertex* screen = new Vertex[vertexCount];

		// Buffer for vertex normals
		Vector* normalBuffer = new Vertex[vertexCount];

		// Process vertices
		model.processVertices(vertexBuffer, time);

		// Transform to world space
		for (int i = 0; i < vertexCount; ++i)
		{
			// Do model view transformation
			cam[i] = operator*(_world.top(), vertexBuffer[i]);
			normalBuffer[i] = _world.top() * vertexBuffer[i].getNormal();

			screen[i] = operator*(_projection.top(), cam[i]);
			
			screen[i](0, 0) /= screen[i](3, 0);
			screen[i](1, 0) /= screen[i](3, 0);
			screen[i](2, 0) /= screen[i](3, 0);
		}

		int frame = model.getCurrentFrame();
		for (int i = 0; i < triangleCount; ++i)
		{
			const Triangle& triangle = triangles[i];

			Vertex& v1 = screen[triangle.A];
			Vertex& v2 = screen[triangle.B];
			Vertex& v3 = screen[triangle.C];
			
			// Cull polygon before the near plane
			if (v1.getZ() < 0 || v2.getZ() < 0 || v3.getZ() < 0 ||
			// or beyond the far plane
				v1.getZ() > 1 || v2.getZ() > 1 || v3.getZ() > 1 ||
			// or if all three vertices are entirely outside of the view
				((v1.getX() < -1 || v1.getX() > 1 || v1.getY() < -1 || v1.getX() > 1) &&
				(v2.getX() < -1 || v2.getX() > 1 || v2.getY() < -1 || v2.getX() > 1) &&
				(v3.getX() < -1 || v3.getX() > 1 || v3.getY() < -1 || v3.getX() > 1)))
				continue;

			Vector normal = _world.top() * triangle.normals[frame];
			
			Vertex& v1cam = cam[triangle.A];
			Vertex& v2cam = cam[triangle.B];
			Vertex& v3cam = cam[triangle.C];

			float cos = normal.dot(v1cam);

			if (cos < 0)
				continue;

			normal.normalise();
			
			Vector normalA = normalBuffer[triangle.A].getNormalised();
			Vector normalB = normalBuffer[triangle.B].getNormalised();
			Vector normalC = normalBuffer[triangle.C].getNormalised();

			float x1 = v1(0, 0) * _width + _width/2.0f;
			float y1 = v1(1, 0) * _height + _height/2.0f;
			float z1 = v1(2, 0);
			float x2 = v2(0, 0) * _width + _width/2.0f;
			float y2 = v2(1, 0) * _height + _height/2.0f;
			float z2 = v2(2, 0);
			float x3 = v3(0, 0) * _width + _width/2.0f;
			float y3 = v3(1, 0) * _height + _height/2.0f;
			float z3 = v3(2, 0);

			const UV* uvs = model.getUVs();

			// Get UV coordinates
			float AU = uvs[triangle.AT].U;
			float AV = uvs[triangle.AT].V;
			float BU = uvs[triangle.BT].U;

			float BV = uvs[triangle.BT].V;
			float CU = uvs[triangle.CT].U;
			float CV = uvs[triangle.CT].V;

			// Camera-space z coordinates
			float AZ = cam[triangle.A].getZ();
			float BZ = cam[triangle.B].getZ();
			float CZ = cam[triangle.C].getZ();
			
			_rasteriser->drawTriangle(x1, y1, z1, v1cam, AU / AZ, AV / AZ, 1 / AZ, normalA,
									x2, y2, z2, v2cam, BU / BZ, BV / BZ, 1 / BZ, normalB,
									x3, y3, z3, v3cam, CU / CZ, CV / CZ, 1/ CZ, normalC,
									model.getTextureCount(), model.getTextures(), _lights);
		}

		// Clean up memory
		delete[] vertexBuffer;
		delete[] normalBuffer;
		delete[] cam;
		delete[] screen;
	}

	void Renderer::setMatrixMode(MatrixMode mode)
	{
		switch (mode)
		{
		case MatrixModes::PROJECTION:
			_matrixStack = &_projection;
			_matrixMode = mode;
			break;
		case MatrixModes::VIEW:
			_matrixStack = &_view;
			_matrixMode = mode;
			break;
		case MatrixModes::WORLD:
		default:
			_matrixStack = &_world;
			_matrixMode = mode;
			break;
		}
	}

	MatrixMode Renderer::getMatrixMode()
	{
		return _matrixMode;
	}
		
	void Renderer::setMaterialType(MaterialType type)
	{
		_materialType = type;
	}

	void Renderer::setShadingType(ShadingType type)
	{
		_shadingType = type;
	}

	void Renderer::setCullingType(CullingType type)
	{
		_cullingType = type;
	}

	void Renderer::addLight(Light* light)
	{
		_lights.push_back(light);
	}

	void Renderer::clearLights()
	{
		_lights.clear();
	}

	void Renderer::pushMatrix()
	{
		// Push current matrix onto the stack
		if (_matrixStack->size())
			_matrixStack->push(_matrixStack->top());
		else
			_matrixStack->push(Matrix4f());
	}

	void Renderer::popMatrix()
	{
		// Pull matrix off top of stack
		_matrixStack->pop();
	}

	Matrix4f Renderer::getMatrix()
	{
		if (_matrixStack->size() > 0)
			return _matrixStack->top();
		else
			return Matrix4f();
	}

	void Renderer::resetMatrixStack()
	{
		// Pop all matrices off stack
		while (!_matrixStack->empty())
			_matrixStack->pop();
	}

	void Renderer::loadIdentity()
	{
		// Push identity matrix onto top of stack
		if (_matrixStack->size() > 0)
			_matrixStack->top() = Matrix4f::createIdentity();
		else
			_matrixStack->push(Matrix4f::createIdentity());
	}

	void Renderer::transform(const Matrix4f& m)
	{
		if (_matrixStack->size() > 0)
			_matrixStack->top() = m * _matrixStack->top();
		else
			_matrixStack->push(m);
	}

	void Renderer::setTarget(Pixel* pixelBuffer, int width, int height)
	{
		if (_rasteriser == 0)
			_rasteriser = new Rasteriser(pixelBuffer, width, height);
		else
			_rasteriser->setTarget(pixelBuffer, width, height);

		_width = width;
		_height = height;
	}

	void Renderer::beginScene(Pixel colour)
	{
		_rasteriser->beginScene(colour);
	}
}
