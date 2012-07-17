#ifndef __DEMO_H__
#define __DEMO_H__

#include <Renderer.h>
#include <MD2_Model.h>

#include <SceneNode.h>
#include <ModelNode.h>
#include <CameraNode.h>

#include <vector>
#include <string>

namespace Demos
{
	class Demo
		: public a3d::SceneNode
	{
	public:
		Demo(a3d::Renderer& rend, a3d::Camera& cam, int time, int length);
		virtual ~Demo();

		virtual void traverse(int time = 0);

		bool dead();
		void reset(int time);

		void addLight(a3d::Light* light);
		void addMessage(const char* message);

		virtual std::vector<std::string> getDemoText();

	protected:
		a3d::Camera& _cam;

	private:
		std::vector<a3d::Light*> _lights;
		std::vector<std::string> _messages;

		a3d::md2::MD2_Model _skybox;

		int _creationTime;
		int _lastUpdate;
		int _length;
	};
}

#endif