#include "Sonic.h"

namespace Demos
{
	Sonic::Sonic(a3d::Renderer& rend, a3d::Camera& cam, int time, int length, a3d::MaterialType materialType,
			a3d::ShadingType shadingType, a3d::CullingType cullingType)
		: Demos::Demo(rend, cam, time, length), _materialType(materialType), _shadingType(shadingType), _cullingType(cullingType)
	{
		// Set up scene
		_model.loadModel("sonic.md2");
		_model.setAnimation(8, 16, 19);

		_tails.loadModel("tails.md2");
		_tails.setAnimation(4, 16, 19);

		a3d::RotatingNode* tailsRotate = new a3d::RotatingNode(rend, 0, 0.001f, 0, 1, time);
		tailsRotate->translate(0, 15, 0);
		add(tailsRotate);

		tailsRotate->add(new a3d::ModelNode(rend, _tails));

		// Load plane
		_plane.loadModel("plane.md2");

		a3d::TransformNode* planeTranslate = new a3d::TransformNode(rend);
		planeTranslate->translate(0, 0, -20);
		planeTranslate->scale(6, 6, 6);
		add(planeTranslate);
		planeTranslate->add(new a3d::ModelNode(rend, _plane));

		// Create node to automatically rotate
		a3d::TransformNode* translate = new a3d::TransformNode(rend);
		translate->translate(-50, 0, 0);
		add(translate);

		a3d::RotatingNode* orbit = new a3d::RotatingNode(rend, 0, 0.005f, 0, 1, time);
		translate->add(orbit);

		// Add cube
		orbit->add(new a3d::ModelNode(rend, _model));
	}

	void Sonic::traverse(int time)
	{
		// Set up camera
		a3d::Vertex4f pos(4, 0, 30, 120, 1);
		_cam.setPosition(pos);

		// Set up rendering mode
		_rend.setMaterialType(_materialType);
		_rend.setShadingType(_shadingType);
		_rend.setCullingType(_cullingType);

		// Render demo
		Demo::traverse(time);
	}
}