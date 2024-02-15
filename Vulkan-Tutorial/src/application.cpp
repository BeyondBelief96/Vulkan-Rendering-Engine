#include "application.h"
#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Trek
{
	Application::Application()
	{
		loadGameObjects();
	}

	void Application::run()
	{
		const SimpleRenderSystem simpleRenderSystem{ trekDevice, trekRenderer.getSwapChainRenderPass() };
		while(!trekWindow.shouldClose())
		{
			glfwPollEvents();
			if(const auto commandBuffer = trekRenderer.beginFrame())
			{
				trekRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				trekRenderer.endSwapChainRenderPass(commandBuffer);
				trekRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(trekDevice.device());
	}

	void Application::loadGameObjects()
	{
		std::vector<TrekModel::Vertex> vertices = triangle();
		const auto model  = std::make_shared<TrekModel>(trekDevice, vertices);

		auto triangle = TrekGameObject::createGameObject();
		triangle.model = model;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 2.f, 0.5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();
		gameObjects.push_back(std::move(triangle));
	}

	std::vector<TrekModel::Vertex>  Application::triangle()
	{
		std::vector<TrekModel::Vertex> vertices{
	  {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
		return vertices;
	}

	void Application::sierpinski(
		std::vector<TrekModel::Vertex>& vertices,
		const int depth,
		const glm::vec2 left,
		const glm::vec2 right,
		const glm::vec2 top) {
		if (depth <= 0) {
			vertices.push_back({ top, {1.0, 0.0, 0.0} });
			vertices.push_back({ right,  {0.0, 1.0, 0.0} });
			vertices.push_back({ left,  {0.0, 0.0, 1.0} });
		}
		else {
			const auto leftTop = 0.5f * (left + top);
			const auto rightTop = 0.5f * (right + top);
			const auto leftRight = 0.5f * (left + right);
			sierpinski(vertices, depth - 1, left, leftRight, leftTop);
			sierpinski(vertices, depth - 1, leftRight, right, rightTop);
			sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	}
}

