#include "application.h"

namespace Trek
{
	Application::Application()
	{
		currentScene = std::make_unique<DiffuseLightingScene>(
			trekWindow,
			trekDevice,
			"shaders/pointlight_diffuse_lighting_ubo_vertex.spv",
			"shaders/pointlight_diffuse_lighting_ubo_fragment.spv");

		currentScene->setup();
	}

	void Application::run()
	{
		currentScene->render();
	}
}

