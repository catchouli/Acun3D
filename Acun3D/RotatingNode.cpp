#include "RotatingNode.h"

namespace a3d
{
	RotatingNode::RotatingNode(Renderer& rend, float rx, float ry, float rz)
		: TransformNode(rend), _rx(rx), _ry(ry), _rz(rz), _speed(0), _lastUpdate(0)
	{

	}

	RotatingNode::RotatingNode(Renderer& rend, float rx, float ry, float rz, float speed, int time)
		: TransformNode(rend), _rx(rx), _ry(ry), _rz(rz), _speed(speed), _lastUpdate(time)
	{

	}

	RotatingNode::~RotatingNode()
	{

	}

	void RotatingNode::setTime(int time)
	{
		_lastUpdate = time;
	}

	void RotatingNode::setRotatingFrequency(float rx, float ry, float rz)
	{
		_rx = rx;
		_ry = ry;
		_rz = rz;
	}

	void RotatingNode::traverse(int time)
	{
		float rot = (time - _lastUpdate) * _speed;
		rotate(_rx * rot, _ry * rot, _rz * rot);
		_lastUpdate = time;
		TransformNode::traverse(time);
	}
}
