#ifndef __SONIC_H__
#define __SONIC_H__

#include <MD2_Model.h>

#include <PointLight.h>

#include <SceneNode.h>
#include <ModelNode.h>
#include <RotatingNode.h>

#include "Demo.h"

namespace Demos
{
	class Sonic
		: public Demo
	{
	public:
		Sonic(a3d::Renderer& rend, a3d::Camera& cam, int time, int length, a3d::MaterialType materialType = a3d::MaterialTypes::SOLID,
			a3d::ShadingType shadingType = a3d::ShadingTypes::FLAT, a3d::CullingType cullingType = a3d::CullingTypes::BACK);
		
		virtual void traverse(int time = 0);

	private:
		a3d::md2::MD2_Model _model;
		a3d::md2::MD2_Model _tails;
		a3d::md2::MD2_Model _plane;
		
		a3d::MaterialType _materialType;
		a3d::ShadingType _shadingType;
		a3d::CullingType _cullingType;
	};
}

#endif