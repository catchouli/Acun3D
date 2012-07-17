#ifndef __CAMERANODE_H__
#define __CAMERANODE_H__

#include "MD2_Model.h"
#include "SceneNode.h"

namespace a3d
{
	class CameraNode
		: public SceneNode
	{
	public:
		CameraNode(Renderer& rend, Camera& cam);
		virtual ~CameraNode();

		void setCamera(Camera& cam);

		virtual void traverse(int time = 0);

	private:
		Camera& _cam;
	};
}

#endif
