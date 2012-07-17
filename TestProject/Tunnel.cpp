#include "Tunnel.h"

namespace Demos
{
	Tunnel::Tunnel(a3d::Renderer& rend, a3d::Camera& cam, int time, int length, a3d::MaterialType materialType,
			a3d::ShadingType shadingType, a3d::CullingType cullingType)
		: Demos::Demo(rend, cam, time, length), _materialType(materialType), _shadingType(shadingType), _cullingType(cullingType)
	{
		// Set up scene
		_cube.loadModel("miku.md2");

		// Create node to automatically rotate
		a3d::TransformNode* transform = new a3d::TransformNode(rend);
		transform->translate(15, 0, -20);
		add(transform);

		a3d::RotatingNode* rotatingNode = new a3d::RotatingNode(rend, 0, 0, 0.0001f, 1, time);
		transform->add(rotatingNode);

		// Add cube
		a3d::ModelNode* modelNode = new a3d::ModelNode(rend, _cube);
		rotatingNode->add(modelNode);

		modelNode->add(transform);
	}

	void Tunnel::traverse(int time)
	{
		// Make camera scroll through miku fractal
		static float z = 80;
		z -= 0.1f;

		if (z < -1337)
			z = 0;

		// Set up camera
		a3d::Vertex4f pos;
		pos(2, 0) = z;
		pos(3, 0) = 1;
		_cam.setPosition(pos);

		// Set up render state
		_rend.setMaterialType(_materialType);
		_rend.setShadingType(_shadingType);
		_rend.setCullingType(_cullingType);

		// Render demo
		Demo::traverse(time);
	}
}