#ifndef APPLICATION_H
#define APPLICATION_H
#include "trek_window.h"
#include "trek_core.h"
#include "trek_renderer.h"
#include "trek_game_object.h"

// std
#include <vector>

#include "AeroWorld2D.h"

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
		void loadGameObjects();
		static std::vector<TrekModel::Vertex> triangle();
		static void sierpinski(
			std::vector<TrekModel::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);

		TrekWindow trekWindow{WIDTH, HEIGHT, "Vulkan Tutorial!"};
		TrekCore trekDevice{ trekWindow };
		TrekRenderer trekRenderer{ trekWindow, trekDevice };
		std::vector<TrekGameObject> gameObjects;
	};
}

#endif
