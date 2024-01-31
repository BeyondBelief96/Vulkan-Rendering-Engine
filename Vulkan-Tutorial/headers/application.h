#ifndef APPLICATION_H
#define APPLICATION_H

#include "hello_triangle_scene.h"

class Application
{
public:
	Application() = default;
	~Application() = default;

	void Run()
	{
		m_scene.InitWindow();
		m_scene.InitVulkan();
		m_scene.MainLoop();
		m_scene.Cleanup();
	}
private:
	HelloTriangleScene m_scene;
};

#endif
