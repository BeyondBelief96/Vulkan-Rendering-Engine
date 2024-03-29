#ifndef APPLICATION_H
#define APPLICATION_H
#include "trek_window.h"
#include "trek_core.h"
#include "trek_renderer.h"
#include "trek_game_object.h"
#include "trek_camera.h"
#include "trek_descriptor_set.h"
#include "scene.h"

// std
#include <memory>
#include <vector>

namespace Trek
{
	class Application
	{
	public:
		Application();
		~Application() = default;
		Application(const Application&) = delete;
		Application& operator=(Application&) = delete;
		Application(const Application&&) = delete;
		Application& operator=(Application&&) = delete;
		void run();
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
	private:
		TrekWindow trekWindow{WIDTH, HEIGHT, "Vulkan Tutorial!"};
		TrekCore trekDevice{ trekWindow };
		std::unique_ptr<Scene> currentScene{};
	};
}

#endif
