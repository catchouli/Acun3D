#include "SceneNode.h"

namespace a3d
{
	SceneNode::SceneNode(Renderer& rend)
		: _rend(rend)
	{

	}
	
	SceneNode::~SceneNode()
	{
		killChildren();
	}
	
	void SceneNode::add(SceneNode* node)
	{
		_children.push_back(node);
	}

	void SceneNode::remove(SceneNode* node)
	{
		_children.remove(node);
	}
	
	void SceneNode::killChildren()
	{
		for (std::list<SceneNode*>::iterator it = _children.begin(); it != _children.end(); it++)
		{
			delete *it;
		}
		_children.clear();
	}

	void SceneNode::draw(int time)
	{
		// Make sure we're in the right matrix mode
		_rend.setMatrixMode(MatrixModes::WORLD);

		// Push a matrix to localise all transformations attached to this node
		_rend.pushMatrix();
			// Call the overloaded traverse method
			traverse(time);
		_rend.popMatrix();
	}
	
	void SceneNode::traverse(int time)
	{
		for (std::list<SceneNode*>::iterator it = _children.begin(); it != _children.end(); it++)
		{
			(*it)->draw(time);
		}
	}
}
