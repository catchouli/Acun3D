#ifndef __SCENENODE_H__
#define __SCENENODE_H__

#include "Renderer.h"

#include <list>

namespace a3d
{
	class SceneNode
	{
	public:
		SceneNode(Renderer& rend);
		virtual ~SceneNode();

		void draw(int time = 0);
		virtual void traverse(int time = 0);

		const SceneNode* getParent() const;

		void add(SceneNode* node);
		void remove(SceneNode* node);
		void killChildren();

	protected:
		SceneNode* _parent;
		std::list<SceneNode*> _children;

		Renderer& _rend;
	};
}

#endif
