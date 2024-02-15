#include "application.h"
#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "gravity_physics_system.h"
#include "pfgen.h"
#include "vec2_field_system.h"

namespace Trek
{
	std::unique_ptr<TrekModel> createSquareModel(TrekCore& device, const glm::vec2 offset) {
		std::vector<TrekModel::Vertex> vertices = {
			{{-0.5f, -0.5f}},
			{{0.5f, 0.5f}},
			{{-0.5f, 0.5f}},
			{{-0.5f, -0.5f}},
			{{0.5f, -0.5f}},
			{{0.5f, 0.5f}},  //
		};
		for (auto& v : vertices) {
			v.pos += offset;
		}
		return std::make_unique<TrekModel>(device, vertices);
	}

	std::unique_ptr<TrekModel> createCircleModel(TrekCore& device, const unsigned int numSides) {
		std::vector<TrekModel::Vertex> uniqueVertices{};
		for (int i = 0; i < numSides; i++) {
			float angle = i * glm::two_pi<float>() / numSides;
			uniqueVertices.push_back({ {glm::cos(angle), glm::sin(angle)} });
		}
		uniqueVertices.push_back({});  // adds center vertex at 0, 0

		std::vector<TrekModel::Vertex> vertices{};
		for (int i = 0; i < numSides; i++) {
			vertices.push_back(uniqueVertices[i]);
			vertices.push_back(uniqueVertices[(i + 1) % numSides]);
			vertices.push_back(uniqueVertices[numSides]);
		}
		return std::make_unique<TrekModel>(device, vertices);
	}

	Application::Application()
	{
		
	}

	void Application::run()
	{
		// Create models
		std::shared_ptr<TrekModel> squareModel = createSquareModel(trekDevice, { .5f, .0f });
		std::shared_ptr<TrekModel> circleModel = createCircleModel(trekDevice, 64);

		// Create physics objects
		std::vector<TrekGameObject> physicsObjects{};
		auto red = TrekGameObject::createGameObject();
		red.transform2d.scale = glm::vec2{ 0.04f };
		red.transform2d.translation = { .5f, .5f };
		red.color = { 1.f, 0.f, 0.f };
		red.particle = std::make_shared<Particle2D>(
			red.transform2d.translation.x,
			red.transform2d.translation.y,
			2.0);
		red.particle->position = { .5f, .5f };
		red.particle->velocity = { -0.1f, 0.1f };
		red.model = circleModel;
		physicsObjects.push_back(std::move(red));
		auto blue = TrekGameObject::createGameObject();
		blue.transform2d.scale = glm::vec2{ 0.03f };
		blue.transform2d.translation = { -.45f, -.25f };
		blue.color = { 0.f, 0.f, 1.f };
		blue.particle = std::make_shared<Particle2D>(
			blue.transform2d.translation.x,
			blue.transform2d.translation.y,
			1.0);
		blue.particle->position = { -.45f, -.25f };
		blue.particle->velocity = { 0.05f, -0.2f };
		blue.model = circleModel;
		physicsObjects.push_back(std::move(blue));

		// Create vector field
		std::vector<TrekGameObject> vectorField{};
		int gridCount = 40;
		for (int i = 0; i < gridCount; i++) {
			for (int j = 0; j < gridCount; j++) {
				auto vf = TrekGameObject::createGameObject();
				vf.transform2d.scale = glm::vec2(0.005f);
				vf.transform2d.translation = {
					-1.0f + (i + 0.5f) * 2.0f / gridCount,
					-1.0f + (j + 0.5f) * 2.0f / gridCount };
				vf.color = glm::vec3(1.0f);
				vf.model = squareModel;
				vectorField.push_back(std::move(vf));
			}
		}

		GravityPhysicsSystem physicsSystem{ physicsObjects };

		const SimpleRenderSystem simpleRenderSystem{ trekDevice, trekRenderer.getSwapChainRenderPass() };
		while(!trekWindow.shouldClose())
		{
			glfwPollEvents();
			if(const auto commandBuffer = trekRenderer.beginFrame())
			{
				// update systems
				physicsSystem.update(1.f / 60);
				Vec2FieldSystem::update(physicsSystem, physicsObjects, vectorField);

				// render system
				trekRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, physicsObjects);
				simpleRenderSystem.renderGameObjects(commandBuffer, vectorField);
				trekRenderer.endSwapChainRenderPass(commandBuffer);
				trekRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(trekDevice.device());
	}
}

