#ifndef TREK_SCENE_H
#define TREK_SCENE_H

#include "trek_core.h"
#include "trek_game_object.h"
#include "trek_renderer.h"
#include "trek_descriptor_set.h"
#include "simple_render_system.h"
#include "keyboard_movement_controller.h"

// std
#include <vector>

namespace Trek
{
	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f }; // w is intensity.
		glm::vec3 lightPosition{ -1.f };
		alignas(16) glm::vec4 lightColor{ 1.f };
	};

	class Scene
	{
	public:

		Scene(
			TrekWindow& trekWindow,
			TrekCore& trekDevice,
			std::string vertexShaderFilePath,
			std::string fragmentShaderFilePath);
		~Scene() = default;

		Scene(const Scene&) = delete;
		Scene& operator=(Scene&) = delete;
		Scene(const Scene&&) = delete;
		Scene& operator=(Scene&&) = delete;

		virtual void setup() = 0;
		virtual void render() = 0;
		virtual void cleanup() = 0;
	protected:
		TrekWindow& trekWindow;
		TrekCore& trekDevice;
		TrekRenderer trekRenderer{ trekWindow, trekDevice };
		std::vector<std::unique_ptr<TrekBuffer>> uboBuffers{ TrekSwapChain::MAX_FRAMES_IN_FLIGHT };
		std::unique_ptr<TrekDescriptorSetLayout> globalDescriptorSetLayout{};
		std::vector<VkDescriptorSet> globalDescriptorSets{ TrekSwapChain::MAX_FRAMES_IN_FLIGHT };

		std::unique_ptr<SimpleRenderSystem> renderSystem;

		TrekGameObject::Map gameObjects;
		TrekCamera camera{};
		TrekGameObject viewerObject = TrekGameObject::createGameObject();
		KeyboardMovementController cameraController{};
		
		std::unique_ptr<TrekDescriptorPool> globalPool{};

		std::string vertexShaderPath;
		std::string fragmentShaderPath;
	};

	class DiffuseLightingScene : public Scene
	{
	public:

		using Scene::Scene;
		~DiffuseLightingScene();

		void setup() override;
		void render() override;
		void cleanup() override;
	};
}

#endif
