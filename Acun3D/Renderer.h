#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <stack>
#include <vector>

#include "MatrixMode.h"
#include "Pixel.h"
#include "Rasteriser.h"
#include "Matrix.h"
#include "MD2_Model.h"
#include "Vertex.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Spotlight.h"
#include "ShadingType.h"
#include "MaterialType.h"
#include "CullingType.h"

namespace a3d
{
	class Renderer
	{
	public:
		Renderer(float nearView = -1, float farView = -1);
		Renderer(Pixel* pixelBuffer, int width, int heigh, float nearView = -1, float farView = -1);
		~Renderer();

		void setTarget(Pixel* pixelBuffer, int _width, int _height);
		void beginScene(Pixel colour);

		bool draw(md2::MD2_Model& model, long time = 0);
		
		void setMatrixMode(MatrixMode mode);
		MatrixMode getMatrixMode();
		
		void setMaterialType(MaterialType type);
		void setShadingType(ShadingType type);
		void setCullingType(CullingType type);

		void addLight(Light* light);
		void removeLight(Light* light);
		void clearLights();
		
		void pushMatrix();
		void popMatrix();
		Matrix4f Renderer::getMatrix();

		void resetMatrixStack();
		
		void loadIdentity();
		void transform(const Matrix4f& m);

	private:
		void drawWireFrame(md2::MD2_Model& model, long time);
		void drawSolidFlat(md2::MD2_Model& model, long time);
		void drawSolidFlatTextured(md2::MD2_Model& model, long time);
		void drawSolidSmooth(md2::MD2_Model& model, long time);
		void drawSolidSmoothTextured(md2::MD2_Model& model, long time);
		void drawSolidPhong(md2::MD2_Model& model, long time);
		void drawSolidPhongTextured(md2::MD2_Model& model, long time);

		Rasteriser* _rasteriser;	
		unsigned int _width;
		unsigned int _height;

		// Clipping distances
		float _nearView;
		float _farView;

		// Rendering modes
		MaterialType _materialType;
		ShadingType _shadingType;
		CullingType _cullingType;
		
		// Static lights for fullbright (_shadingType == ShadingTypes::NONE)
		AmbientLight _fullBright;
		std::vector<Light*> _full;

		// Light in the scene
		std::vector<Light*> _lights;

		// Current Matrix Stack
		std::stack<Matrix4f>* _matrixStack;

		// Current Matrix Mode
		MatrixMode _matrixMode;

		// Matrix stacks
		std::stack<Matrix4f> _world;
		std::stack<Matrix4f> _view;
		std::stack<Matrix4f> _projection;
	};
}

#endif