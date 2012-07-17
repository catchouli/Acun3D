#include "CameraRotationNode.h"

namespace a3d
{
	CameraRotationNode::CameraRotationNode(Renderer& rend, Camera& cam)
		: SceneNode(rend), _cam(cam)
	{

	}

	CameraRotationNode::~CameraRotationNode()
	{

	}

	void CameraRotationNode::setCamera(Camera& camera)
	{
		_cam = camera;
	}

	void CameraRotationNode::traverse(int time)
	{
		// Update camera rotation
		Matrix4f world = _rend.getMatrix();
		
		_cam.setRotation(0, 0, 0);
		_cam.rotate(world);

		SceneNode::traverse(time);	
	}
}
