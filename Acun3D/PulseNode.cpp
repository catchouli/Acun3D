#include "PulseNode.h"

namespace a3d
{
	PulseNode::PulseNode(Renderer& rend, float sx1, float sy1, float sz1, float sx2, float sy2, float sz2)
		: TransformNode(rend), _sx1(sx1), _sy1(sy1), _sz1(sz1), _sx2(sx2), _sy2(sy2), _sz2(sz2),
			_sxc(sx1), _syc(sy1), _szc(sz1), _sm(1), _speed(0), _lastUpdate(0)
	{

	}

	PulseNode::PulseNode(Renderer& rend, float sx1, float sy1, float sz1, float sx2, float sy2, float sz2, float speed, int time)
		: TransformNode(rend), _sx1(sx1), _sy1(sy1), _sz1(sz1), _sx2(sx2), _sy2(sy2), _sz2(sz2),
			_sxc(sx1), _syc(sy1), _szc(sz1), _sm(1), _speed(speed), _lastUpdate(time)
	{

	}

	PulseNode::PulseNode(Renderer& rend, float s1, float s2)
		: TransformNode(rend), _sx1(s1), _sy1(s1), _sz1(s1), _sx2(s2), _sy2(s2), _sz2(s2),
			_sxc(s1), _syc(s1), _szc(s1), _sm(1), _speed(0), _lastUpdate(0)
	{

	}

	PulseNode::PulseNode(Renderer& rend, float s1, float s2, float speed, int time)
		: TransformNode(rend), _sx1(s1), _sy1(s1), _sz1(s1), _sx2(s2), _sy2(s2), _sz2(s2),
			_sxc(s1), _syc(s1), _szc(s1), _sm(1), _speed(speed), _lastUpdate(time)
	{

	}

	PulseNode::~PulseNode()
	{

	}

	void PulseNode::setTime(int time)
	{
		_lastUpdate = time;
	}

	void PulseNode::setPulseFrequency(float s1, float s2)
	{
		_sx1 = s1;
		_sy1 = s1;
		_sz1 = s1;
		_sx2 = s2;
		_sy2 = s2;
		_sz2 = s2;
	}

	void PulseNode::setPulseFrequency(float sx1, float sy1, float sz1,
									float sx2, float sy2, float sz2)
	{
		_sx1 = sx1;
		_sy1 = sy1;
		_sz1 = sz1;
		_sx2 = sx2;
		_sy2 = sy2;
		_sz2 = sz2;
	}

	template <class T>
	T min(T a, T b)
	{
		return a < b ? a : b;
	}

	template <class T>
	T max(T a, T b)
	{
		return a > b ? a : b;
	}

	void PulseNode::traverse(int time)
	{
		float scaleFactor = 1;

		if (time != 0)
		{
			scaleFactor = (time - _lastUpdate) * _speed;
			_lastUpdate = time;
		}
			
		_sxc += scaleFactor * _sm * (_sx1 - _sx2);
		_syc += scaleFactor * _sm * (_sy1 - _sx2);
		_szc += scaleFactor * _sm * (_sz1 - _sz2);
			
		if (_sxc < min(_sx1, _sx2) || _syc < min(_sy1, _sy2) || _szc < min(_sz1, _sz2) ||
			_sxc > max(_sx1, _sx2) || _syc > max(_sy1, _sy2) || _szc > max(_sz1, _sz2))
			_sm *= -1;
			
		if (_sxc < min(_sx1, _sx2))
			_sxc = min(_sx1, _sx2);
		if (_syc < min(_sy1, _sy2))
			_syc = min(_sy1, _sy2);
		if (_szc < min(_sz1, _sz2))
			_szc = min(_sz1, _sz2);
		if (_sxc > max(_sx1, _sx2))
			_sxc = max(_sx1, _sx2);
		if (_syc > max(_sy1, _sy2))
			_syc = max(_sy1, _sy2);
		if (_szc > max(_sz1, _sz2))
			_szc = max(_sz1, _sz2);

		setScale(_sxc, _syc, _szc);
		_lastUpdate = time;
		TransformNode::traverse(time);
	}
}
