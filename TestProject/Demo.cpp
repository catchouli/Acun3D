#include "Demo.h"

namespace Demos
{
	Demo::Demo(a3d::Renderer& rend, a3d::Camera& cam, int time, int length)
		: a3d::SceneNode(rend), _cam(cam), _creationTime(time), _lastUpdate(time), _length(length)
	{

	}
	
	Demo::~Demo()
	{
		for (unsigned int i = 0; i < _lights.size(); ++i)
		{
			delete _lights[i];
		}
	}

	void Demo::traverse(int time)
	{
		// Set up view matrix
		_rend.setMatrixMode(a3d::MatrixModes::VIEW);
		_rend.loadIdentity();
		
		// Transform it to the camera
		_rend.transform(a3d::Matrix4f::createTranslation(-_cam.getPosition()(0, 0), -_cam.getPosition()(1, 0), -_cam.getPosition()(2, 0)));
		_rend.transform(_cam.getRotation());

		// Switch matrix mode back to world so nothing messes up during traversal
		_rend.setMatrixMode(a3d::MatrixModes::WORLD);

		_lastUpdate = time;

		_rend.clearLights();

		for (unsigned int i = 0; i < _lights.size(); ++i)
		{
			_rend.addLight(_lights[i]);
		}

		a3d::SceneNode::traverse(time);
	}

	bool Demo::dead()
	{
		return (_lastUpdate - _creationTime > _length);
	}

	void Demo::reset(int time)
	{
		_creationTime = time;
		_lastUpdate = time;
		
		_cam.setPosition(a3d::Vertex4f(4, 0, 0, 0, 1));
		_cam.setRotation(0, 0, 0);
	}

	void Demo::addLight(a3d::Light* light)
	{
		_lights.push_back(light);
	}

	void Demo::addMessage(const char* message)
	{
		_messages.push_back(message);
	}

	std::vector<std::string> Demo::getDemoText()
	{
		return _messages;
	}
}