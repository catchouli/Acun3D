#include "Miku.h"

namespace Demos
{
	Miku::Miku(a3d::Renderer& rend, a3d::Camera& cam, int time, int length, a3d::MaterialType materialType,
			a3d::ShadingType shadingType, a3d::CullingType cullingType, const char* textureName)
		: Demos::Demo(rend, cam, time, length), _materialType(materialType), _shadingType(shadingType), _cullingType(cullingType)
	{
		// Set up scene
		_model.loadModel("miku.md2");

		// Create node to automatically rotate
		a3d::RotatingNode* rotatingNode = new a3d::RotatingNode(rend, 0, 0.001f, 0, 1, time);
		add(rotatingNode);

		// Add cube
		rotatingNode->add(new a3d::ModelNode(rend, _model));
	}

	void Miku::traverse(int time)
	{
		// Set up camera
		a3d::Vertex4f pos(4, 0, 30, 80, 1);
		_cam.setPosition(pos);

		// Set up rendering mode
		_rend.setMaterialType(_materialType);
		_rend.setShadingType(_shadingType);
		_rend.setCullingType(_cullingType);

		// Render demo
		Demo::traverse(time);
	}
}