#ifndef __TRANSFORMNODE_H__
#define __TRANSFORMNODE_H__

#include "SceneNode.h"
#include "Matrix.h"

namespace a3d
{
	class TransformNode
		: public SceneNode
	{
	public:
		TransformNode(Renderer& rend);
		virtual ~TransformNode();

		void setTranslate(float x, float y, float z);
		void setRotate(float x, float y, float z);
		void setScale(float x, float y, float z);

		void translate(float x, float y, float z);
		void rotate(float x, float y, float z);
		void scale(float x, float y, float z);

		virtual void traverse(int time);

	private:
		float _tx, _ty, _tz;
		float _rx, _ry, _rz;
		float _sx, _sy, _sz;
	};
}

#endif /* __TRANSFORMNODE_H__ */
