#include "ModelNode.h"

namespace a3d
{
	ModelNode::ModelNode(Renderer& rend, md2::MD2_Model& model)
		: SceneNode(rend), _model(model)
	{

	}

	ModelNode::~ModelNode()
	{

	}

	void ModelNode::setModel(md2::MD2_Model& model)
	{
		_model = model;
	}

	void ModelNode::traverse(int time)
	{
		// Don't traverse unless the model successfully drew (for recursive scenes)
		if (_rend.draw(_model, time))
			SceneNode::traverse(time);
	}
}
