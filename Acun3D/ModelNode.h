#ifndef __MODELNODE_H__
#define __MODELNODE_H__

#include "MD2_Model.h"
#include "SceneNode.h"

namespace a3d
{
	class ModelNode
		: public SceneNode
	{
	public:
		ModelNode(Renderer& rend, md2::MD2_Model& model);
		virtual ~ModelNode();

		void setModel(md2::MD2_Model& model);

		virtual void traverse(int time = 0);

	private:
		md2::MD2_Model& _model;
	};
}

#endif
