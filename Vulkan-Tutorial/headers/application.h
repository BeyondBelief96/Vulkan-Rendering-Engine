#ifndef APPLICATION_H
#define APPLICATION_H
#include "trek_window.h"
#include "trek_core.h"
#include "trek_renderer.h"
#include "trek_game_object.h"
#include "trek_camera.h"
#include "trek_descriptor_set.h"

// std
#include <memory>
#include <vector>

namespace Trek
{
	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f }; // w is intensity.
		glm::vec3 lightPosition{ -1.f };
		alignas(16) glm::vec4 lightColor{ 1.f };
	};

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

		TrekWindow trekWindow{WIDTH, HEIGHT, "Vulkan Tutorial!"};
		TrekCore trekDevice{ trekWindow };
		TrekRenderer trekRenderer{ trekWindow, trekDevice };

		std::unique_ptr<TrekDescriptorPool> globalPool{};
		std::vector<TrekGameObject> gameObjects;
	};
}

#endif
