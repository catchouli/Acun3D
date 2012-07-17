#ifndef __CAMERAROTATIONNODE_H__
#define __CAMERAROTATIONNODE_H__

#include "MD2_Model.h"
#include "SceneNode.h"

namespace a3d
{
	class CameraRotationNode
		: public SceneNode
	{
	public:
		CameraRotationNode(Renderer& rend, Camera& cam);
		virtual ~CameraRotationNode();

		void setCamera(Camera& cam);

		virtual void traverse(int time = 0);

	private:
		Camera& _cam;
	};
}

#endif
