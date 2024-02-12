#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

#include "application.h"

#include <chrono>

#include "shaders.h"
#include "trek_pipeline.h"
#include "trek_swap_chain.h"

namespace Trek
{
	void Application::InitVulkan()
	{
		CreateDescriptorSets();
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	void Application::MainLoop()
	{
		while (!glfwWindowShouldClose(m_glfw.GetWindow()))
		{
			glfwPollEvents();
			DrawFrame();
		}

		vkDeviceWaitIdle(m_core.GetDevice());
	}

	void Application::Cleanup() const
	{
		vkDestroyRenderPass(m_core.GetDevice(), m_trekSwapChain->GetRenderPass(), nullptr);
		vkDestroyPipelineLayout(m_core.GetDevice(), m_pipelineLayout, nullptr);
	}


	void Application::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(
			m_core.GetDevice(),
			m_core.GetGraphicsCmdPool(),
			static_cast<uint32_t>(m_commandBuffers.size()),
			m_commandBuffers.data());

		m_commandBuffers.clear();
	}

	void Application::CreateDescriptorSets()
	{
		m_globalSetLayout = TrekDescriptorSetLayout::Builder(m_core)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		m_globalPool = TrekDescriptorPool::Builder(m_core)
			.SetMaxSets(TrekSwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, TrekSwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		for (int i = 0; i < TrekSwapChain::MAX_FRAMES_IN_FLIGHT; i++)
		{
			auto bufferInfo = m_globalUboBuffers[i]->DescriptorInfo();
			VkDescriptorSet globalDescriptorSet{};
			TrekDescriptorWriter(*m_globalSetLayout, *m_globalPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSet);

			m_globalDescriptorSets.push_back(globalDescriptorSet);

		}
	}

	void Application::CreatePipelineLayout()
	{
		const std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ m_globalSetLayout->GetDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo;
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		pipelineLayoutInfo.flags = VK_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT;
		pipelineLayoutInfo.pNext = nullptr;
		if(vkCreatePipelineLayout(m_core.GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout)
			!= VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void Application::CreatePipeline()
	{
		assert(m_trekSwapChain != nullptr && "Cannot create pipeline before swap chain.");
		assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");
		PipelineConfigInfo pipelineConfig{};
		TrekPipeline::DefaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.render_pass = m_trekSwapChain->GetRenderPass();
		pipelineConfig.pipeline_layout = m_pipelineLayout;
		m_trekPipeline = std::make_unique<TrekPipeline>(
			m_core,
			"shaders/vert.spv",
			"shaders/frag.spv",
			pipelineConfig);
	}

	void Application::RecreateSwapChain()
	{
		auto extent = m_glfw.GetExtent();
		while ((extent.width == 0 || extent.height == 0) && !glfwWindowShouldClose(m_glfw.GetWindow())) {

			extent = m_glfw.GetExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(m_core.GetDevice());
		if(m_trekSwapChain == nullptr)
		{
			m_trekSwapChain = std::make_unique<TrekSwapChain>(m_core, m_glfw);
		}
		else
		{
			m_trekSwapChain = std::make_unique<TrekSwapChain>(
				m_core,
				m_glfw,
				std::move(m_trekSwapChain));
			if(m_trekSwapChain->ImageCount() != m_commandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}
		
		CreatePipeline();
	}

	void Application::CreateCommandBuffers()
	{
		m_commandBuffers.resize(TrekSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_core.GetGraphicsCmdPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

		if (vkAllocateCommandBuffers(m_core.GetDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void Application::RecordCommandBuffer(const uint32_t imageIndex) const
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_trekSwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_trekSwapChain->GetFrameBuffer(static_cast<int>(imageIndex));
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_trekSwapChain->GetSwapChainExtent();
		const VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		m_trekPipeline->Bind(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()]);
		m_model->Bind(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()]);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_trekSwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_trekSwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_trekSwapChain->GetSwapChainExtent();
		vkCmdSetScissor(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()], 0, 1, &scissor);
		vkCmdBindDescriptorSets(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout,
			0, 1, &m_globalDescriptorSets[m_trekSwapChain->GetCurrentFrame()], 0, nullptr);
		m_model->Draw(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()]);
		vkCmdEndRenderPass(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()]);
		if (vkEndCommandBuffer(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void Application::UpdateUniformBuffers(const uint32_t currentImage) const
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		const auto currentTime = std::chrono::high_resolution_clock::now();
		const float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime -
			startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f),
			m_trekSwapChain->ExtentAspectRatio(), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		m_globalUboBuffers[currentImage]->WriteToBuffer(&ubo);
		const VkResult result = m_globalUboBuffers[currentImage]->Flush(); 
		if(result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to update uniform buffer!");
		}
		
	}

	void Application::LoadModels()
	{
		m_model = CreateCubeModel({0.0f, 0.0f, 0.0f});
	}

	std::unique_ptr<TrekModel> Application::CreateCubeModel(const glm::vec3 offset)
	{
		TrekModel::Data modelData{};
		modelData.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y-axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};

		for (auto& v : modelData.vertices) {
			v.pos += offset;
		}

		modelData.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<TrekModel>(m_core, modelData);
	}

	void Application::DrawFrame()
	{
		uint32_t imageIndex;
		VkResult result = m_trekSwapChain->AcquireNextImage(&imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		vkResetCommandBuffer(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()], 0);
		RecordCommandBuffer(imageIndex);
		UpdateUniformBuffers(static_cast<uint32_t>(m_trekSwapChain->GetCurrentFrame()));

		result = m_trekSwapChain->SubmitCommandBuffers(m_commandBuffers[m_trekSwapChain->GetCurrentFrame()], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_glfw.HasFrameBufferResized()) {
			m_glfw.SetFrameBufferResized(false);
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

	Application::Application()
	{
		for (auto& globalUboBuffer : m_globalUboBuffers)
		{
			globalUboBuffer = std::make_unique<TrekBuffer>(
				m_core,
				sizeof(UniformBufferObject),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			globalUboBuffer->Map();
		}
		LoadModels();
	}

	void Application::Run()
	{
		InitVulkan();
		MainLoop();
		Cleanup();
	}
}

