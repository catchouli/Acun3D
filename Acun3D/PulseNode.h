#ifndef __PULSENODE_H__
#define __PULSENODE_H__

#include "TransformNode.h"

namespace a3d
{
	class PulseNode
		: public TransformNode
	{
	public:
		PulseNode::PulseNode(Renderer& rend, float sx1, float sy1, float sz1, float sx2, float sy2, float sz2);
		PulseNode::PulseNode(Renderer& rend, float sx1, float sy1, float sz1, float sx2, float sy2, float sz2, float speed, int time);
		PulseNode::PulseNode(Renderer& rend, float s1, float s2);
		PulseNode::PulseNode(Renderer& rend, float s1, float s2, float speed, int time);
		~PulseNode();

		void setTime(int time);
		void setPulseFrequency(float s1, float s2);
		void setPulseFrequency(float sx1, float sy1, float sz1, float sx2, float sy2, float sz2);;

		void traverse(int time);

	private:
		// Initial scale
		float _sx1;
		float _sy1;
		float _sz1;

		// Final scale
		float _sx2;
		float _sy2;
		float _sz2;

		// Current scale
		float _sxc;
		float _syc;
		float _szc;

		// Current scale multiplier
		float _sm;

		float _speed;
		int _lastUpdate;
	};
}

#endif
