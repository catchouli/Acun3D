#ifndef __TRANSLATINGNODE_H__
#define __TRANSLATINGNODE_H__

#include "TransformNode.h"

namespace a3d
{
	class TranslatingNode
		: public TransformNode
	{
	public:
		TranslatingNode(Renderer& rend, float tx, float ty, float tz);
		TranslatingNode(Renderer& rend, float tx, float ty, float tz, float speed, int time);
		~TranslatingNode();

		void setTime(int time);
		void setTranslatingFrequency(float tx, float ty, float tz);

		void traverse(int time = 0);

	private:
		float _tx;
		float _ty;
		float _tz;

		float _speed;
		int _lastUpdate;
	};
}

#endif

