#include "CameraNode.h"

namespace a3d
{
	CameraNode::CameraNode(Renderer& rend, Camera& cam)
		: SceneNode(rend), _cam(cam)
	{

	}

	CameraNode::~CameraNode()
	{

	}

	void CameraNode::setCamera(Camera& camera)
	{
		_cam = camera;
	}

	void CameraNode::traverse(int time)
	{
		// Update camera position
		Matrix4f world = _rend.getMatrix();
		
		float x = world(0, 3);
		float y = world(1, 3);
		float z = world(2, 3);
		float w = world(3, 3);

		a3d::Vertex4f v;
		v(0, 0) = x;
		v(1, 0) = y;
		v(2, 0) = z;
		v(3, 0) = w;
		_cam.setPosition(v);

		SceneNode::traverse(time);	
	}
}
