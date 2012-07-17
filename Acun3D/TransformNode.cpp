#include "TransformNode.h"

namespace a3d
{
	TransformNode::TransformNode(Renderer& rend)
		: SceneNode(rend)
	{
		setTranslate(0, 0, 0);
		setRotate(0, 0, 0);
		setScale(1, 1, 1);
	}

	TransformNode::~TransformNode()
	{

	}

	void TransformNode::setTranslate(float x, float y, float z)
	{
		_tx = x;
		_ty = y;
		_tz = z;
	}

	void TransformNode::setRotate(float x, float y, float z)
	{
		_rx = x;
		_ry = y;
		_rz = z;
	}

	void TransformNode::setScale(float x, float y, float z)
	{
		_sx = x;
		_sy = y;
		_sz = z;
	}

	void TransformNode::translate(float x, float y, float z)
	{
		_tx += x;
		_ty += y;
		_tz += z;
	}

	void TransformNode::rotate(float x, float y, float z)
	{
		_rx += x;
		_ry += y;
		_rz += z;
	}

	void TransformNode::scale(float x, float y, float z)
	{
		_sx *= x;
		_sy *= y;
		_sz *= z;
	}

	void TransformNode::traverse(int time)
	{
		Matrix4f scale = Matrix4f::createScale(_sx, _sy, _sz);

		_rend.transform(scale);
		_rend.transform(Matrix4f::createRotationX(_rx));
		_rend.transform(Matrix4f::createRotationY(_ry));
		_rend.transform(Matrix4f::createRotationZ(_rz));
		_rend.transform(Matrix4f::createTranslation(_tx, _ty, _tz));

		SceneNode::traverse(time);
	}
}
