#include "TranslatingNode.h"

namespace a3d
{
	TranslatingNode::TranslatingNode(Renderer& rend, float tx, float ty, float tz)
		: TransformNode(rend), _tx(tx), _ty(ty), _tz(tz), _speed(0), _lastUpdate(0)
	{

	}

	TranslatingNode::TranslatingNode(Renderer& rend, float tx, float ty, float tz, float speed, int time)
		: TransformNode(rend), _tx(tx), _ty(ty), _tz(tz), _speed(speed), _lastUpdate(time)
	{

	}

	TranslatingNode::~TranslatingNode()
	{

	}

	void TranslatingNode::setTime(int time)
	{
		_lastUpdate = time;
	}

	void TranslatingNode::setTranslatingFrequency(float tx, float ty, float tz)
	{
		_tx = tx;
		_ty = ty;
		_tz = tz;
	}

	void TranslatingNode::traverse(int time)
	{
		float rot = (time - _lastUpdate) * _speed;
		translate(_tx * rot, _ty * rot, _tz * rot);
		_lastUpdate = time;
		TransformNode::traverse(0);
	}
}
