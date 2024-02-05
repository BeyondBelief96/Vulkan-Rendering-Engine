#include "application.h"

#include "pipeline.h"
#include "shaders.h"
#include "util.h"

void Application::InitVulkan()
{
	m_core.Init(&m_glfw);
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFrameBuffers();
	CreateCommandPool();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateCommandBuffers();
	CreateSyncObjects();
}

void Application::MainLoop()
{
	while (!glfwWindowShouldClose(m_glfw.GetWindow()))
	{
		glfwPollEvents();
		DrawFrame();
	}

	vkDeviceWaitIdle(m_device);
}

void Application::Cleanup() const
{
	CleanupSwapChain();
	vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
	vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);
	vkDestroyBuffer(m_device, m_indexBuffer, nullptr);
	vkFreeMemory(m_device, m_indexBufferMemory, nullptr);
	vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	vkDestroyRenderPass(m_device, m_renderPass, nullptr);

	for (size_t i = 0; i < m_max_Frames_In_Flight; i++) {
		vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(m_device, m_graphicsCommandPool, nullptr);
	vkDestroyCommandPool(m_device, m_transferCommandPool, nullptr);
	vkDestroyDevice(m_device, nullptr);

	m_core.Cleanup();
	m_glfw.Cleanup();
}

void Application::CreateLogicalDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	auto indices = m_core.GetQueueFamilies();
	const std::set uniqueQueueFamilies = { indices.graphics_family.value(), indices.present_family.value(), indices.transfer_family.value() };

	const float queuePriority = 1.0f;
	for (const uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	const VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
	createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_core.GetPhysicalDevice(), &createInfo, nullptr, &m_device) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(m_device, indices.graphics_family.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(m_device, indices.present_family.value(), 0, &m_presentQueue);
	vkGetDeviceQueue(m_device, indices.transfer_family.value(), 0, &m_transferQueue);
}

void Application::CreateSwapChain()
{
	const VkPhysicalDevice physicalDevice = m_core.GetPhysicalDevice();
	const VkSurfaceKHR surface = m_core.GetSurface();
	const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);

	const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.present_modes);
	const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, m_glfw.GetWindow());

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_core.GetSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	const QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
	const uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value() };

	if (indices.graphics_family != indices.present_family) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}
	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());

	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;
}

void Application::RecreateSwapChain(const VkDevice& device)
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(m_glfw.GetWindow(), &width, &height);
	while ((width == 0 || height == 0) && !glfwWindowShouldClose(m_glfw.GetWindow())) {

		glfwGetFramebufferSize(m_glfw.GetWindow(), &width, &height);
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(device);
	CleanupSwapChain();
	CreateSwapChain();
	CreateImageViews();
	CreateFrameBuffers();
}

void Application::CleanupSwapChain() const
{
	for (const auto framebuffer : m_swapChainFramebuffers) {
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
	}

	for (const auto imageView : m_swapChainImageViews) {
		vkDestroyImageView(m_device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
}

void Application::CreateImageViews()
{
	m_swapChainImageViews.resize(m_swapChainImages.size());
	for (auto i = 0; i < m_swapChainImages.size(); ++i)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void Application::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0; dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void Application::CreateGraphicsPipeline()
{
	const auto vertShaderCode = ReadFile("shaders/vert.spv");
	const auto fragShaderCode = ReadFile("shaders/frag.spv");
	const VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode, m_device);
	const VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode, m_device);

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(DYNAMIC_STATES.size());
	dynamicState.pDynamicStates = DYNAMIC_STATES.data();

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	auto bindingDescription = Vertex::GetBindingDescription();
	auto attributeDescriptions = Vertex::GetAttributeDescriptions();

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
}

void Application::CreateFrameBuffers()
{
	m_swapChainFramebuffers.resize(m_swapChainImageViews.size());
	for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			m_swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swapChainExtent.width;
		framebufferInfo.height = m_swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void Application::CreateCommandPool()
{
	VkCommandPoolCreateInfo graphicsPoolInfo{};
	graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	graphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	graphicsPoolInfo.queueFamilyIndex = m_core.GetQueueFamilies().graphics_family.value();

	VkCommandPoolCreateInfo transferPoolInfo{};
	transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	transferPoolInfo.queueFamilyIndex = m_core.GetQueueFamilies().transfer_family.value();


	if (vkCreateCommandPool(m_device, &graphicsPoolInfo, nullptr, &m_graphicsCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}

	if (vkCreateCommandPool(m_device, &transferPoolInfo, nullptr, &m_transferCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void Application::CreateVertexBuffer()
{
	const VkDeviceSize bufferSize = sizeof(VERTICES[0]) * VERTICES.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	m_core.CreateBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	
	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, VERTICES.data(), (size_t)bufferSize);
	vkUnmapMemory(m_device, stagingBufferMemory);

	m_core.CreateBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

	VulkanCore::CopyBuffer(m_device, bufferSize, stagingBuffer, m_vertexBuffer, m_transferCommandPool,
		m_transferQueue);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);

}

void Application::CreateIndexBuffer()
{
	const VkDeviceSize bufferSize = sizeof(INDICES[0]) * INDICES.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	m_core.CreateBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, INDICES.data(), (size_t)bufferSize);
	vkUnmapMemory(m_device, stagingBufferMemory);

	m_core.CreateBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

	VulkanCore::CopyBuffer(m_device, bufferSize, stagingBuffer, m_indexBuffer, m_transferCommandPool,
		m_transferQueue);

	vkDestroyBuffer(m_device, stagingBuffer, nullptr);
	vkFreeMemory(m_device, stagingBufferMemory, nullptr);
}

void Application::CreateCommandBuffers()
{
	m_commandBuffers.resize(m_max_Frames_In_Flight);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_graphicsCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

	if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Application::CreateSyncObjects()
{
	m_imageAvailableSemaphores.resize(m_max_Frames_In_Flight);
	m_renderFinishedSemaphores.resize(m_max_Frames_In_Flight);
	m_inFlightFences.resize(m_max_Frames_In_Flight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < m_max_Frames_In_Flight; ++i)
	{

		if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores!");
		}
	}

}

void Application::RecordCommandBuffer(const uint32_t imageIndex) const
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(m_commandBuffers[m_current_Frame], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass;
	renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapChainExtent;
	const VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(m_commandBuffers[m_current_Frame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(m_commandBuffers[m_current_Frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

	const VkBuffer vertexBuffers[] = { m_vertexBuffer };
	const VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(m_commandBuffers[m_current_Frame], 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(m_commandBuffers[m_current_Frame], m_indexBuffer, 0, VK_INDEX_TYPE_UINT16);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swapChainExtent.width);
	viewport.height = static_cast<float>(m_swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_commandBuffers[m_current_Frame], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChainExtent;
	vkCmdSetScissor(m_commandBuffers[m_current_Frame], 0, 1, &scissor);
	vkCmdDrawIndexed(m_commandBuffers[m_current_Frame], static_cast<uint32_t>(INDICES.size()), 1, 0, 0, 0);
	vkCmdEndRenderPass(m_commandBuffers[m_current_Frame]);
	if (vkEndCommandBuffer(m_commandBuffers[m_current_Frame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void Application::DrawFrame()
{
	vkWaitForFences(m_device, 1, &m_inFlightFences[m_current_Frame], VK_TRUE, UINT64_MAX);
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_current_Frame],
		VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain(m_device);
		return;
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	// Only reset the fence if we are submitting work
	vkResetFences(m_device, 1, &m_inFlightFences[m_current_Frame]);

	vkResetCommandBuffer(m_commandBuffers[m_current_Frame], 0);
	RecordCommandBuffer(imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	const VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_current_Frame] };
	const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[m_current_Frame];
	const VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_current_Frame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_current_Frame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	const VkSwapchainKHR swapChains[] = { m_swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_glfw.HasFrameBufferResized()) {
		m_glfw.SetFrameBufferResized(false);
		RecreateSwapChain(m_device);
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}
	m_current_Frame = (m_current_Frame + 1) % m_max_Frames_In_Flight;
}


void Application::Run()
{
	m_glfw.Init(800, 600, "Vulkan Tutorial");
	InitVulkan();
	MainLoop();
	Cleanup();
}
