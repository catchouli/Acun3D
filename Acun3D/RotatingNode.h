#ifndef __ROTATINGNODE_H__
#define __ROTATINGNODE_H__

#include "TransformNode.h"

namespace a3d
{
	class RotatingNode
		: public TransformNode
	{
	public:
		RotatingNode(Renderer& rend, float rx, float ry, float rz);
		RotatingNode(Renderer& rend, float rx, float ry, float rz, float speed, int time);
		~RotatingNode();

		void setTime(int time);
		void setRotatingFrequency(float rx, float ry, float rz);

		void traverse(int time = 0);

	private:
		float _rx;
		float _ry;
		float _rz;

		float _speed;
		int _lastUpdate;
	};
}

#endif

