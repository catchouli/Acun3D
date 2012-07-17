#include "Orbit.h"

namespace Demos
{
	Orbit::Orbit(a3d::Renderer& rend, a3d::Camera& cam, int time, int length, a3d::MaterialType materialType,
			a3d::ShadingType shadingType, a3d::CullingType cullingType)
		: Demos::Demo(rend, cam, time, length), _materialType(materialType), _shadingType(shadingType), _cullingType(cullingType)
	{
		// Set up scene
		_sphere.loadModel("test.md2");
		_model.loadModel("miku.md2");

		// Scale the sun and make it rotate
		a3d::RotatingNode* sunRotation = new a3d::RotatingNode(rend, 0.01f, 0.01f, 0, 1, time);
		add(sunRotation);

		a3d::TransformNode* moonTranslate = new a3d::TransformNode(rend);
		moonTranslate->translate(0, 75, 0);
		moonTranslate->scale(0.1f, 0.1f, 0.1f);
		add(moonTranslate);

		a3d::RotatingNode* moonRotation = new a3d::RotatingNode(rend, 0.01f, 0, 0, 0.1f, time);
		moonTranslate->add(moonRotation);

		a3d::TransformNode* moonRotate = new a3d::TransformNode(rend);
		moonRotate->rotate(0, 0, 0.5f);
		moonRotation->add(moonRotate);

		a3d::ModelNode* moon = new a3d::ModelNode(rend, _sphere);
		moonRotate->add(moon);

		a3d::TransformNode* scale = new a3d::TransformNode(rend);
		scale->scale(0.25f, 0.25f, 0.25f);
		sunRotation->add(scale);

		scale->add(new a3d::ModelNode(rend, _sphere));

		// Rotate miku to be on her side
		a3d::TransformNode* rotation = new a3d::TransformNode(rend);
		rotation->rotate(2.0f, 0, 3.14159f/2.0f);
		add(rotation);

		a3d::TransformNode* translate = new a3d::TransformNode(rend);
		translate->translate(-50, 0, 0);
		rotation->add(translate);

		a3d::RotatingNode* orbit = new a3d::RotatingNode(rend, 0, 0.001f, 0, 1, time);
		translate->add(orbit);

		// Add cube
		orbit->add(new a3d::ModelNode(rend, _model));
	}

	void Orbit::traverse(int time)
	{
		// Set up camera
		a3d::Vertex4f pos(4, 0, 0, 150, 1);
		_cam.setPosition(pos);

		// Set up rendering mode
		_rend.setMaterialType(_materialType);
		_rend.setShadingType(_shadingType);
		_rend.setCullingType(_cullingType);

		// Render demo
		Demo::traverse(time);
	}
}