#ifndef HELLO_TRIANGLE_SCENE_H
#define HELLO_TRIANGLE_SCENE_H
#include "Scene.h"

class HelloTriangleScene : public Scene
{
public:
	virtual void InitWindow() override;
	virtual void InitVulkan() override;
	virtual void MainLoop() override;
	virtual void Cleanup() override;

protected:
	virtual void DrawFrame() override;
	virtual void CreateInstance() override;
	virtual void SetupDebugMessenger() override;
	virtual void CreateSurface() override;
	virtual void PickPhysicalDevice() override;
	virtual void CreateLogicalDevice() override;
	virtual void CreateSwapChain() override;
	virtual void CreateImageViews() override;
	virtual void CreateRenderPass() override;
	virtual void CreateGraphicsPipeline() override;
	virtual void CreateFrameBuffers() override;
	virtual void CreateCommandPool() override;
	virtual void CreateCommandBuffers() override;
	virtual void CreateSyncObjects() override;
	virtual void RecreateSwapChain(const VkDevice& device) override;
	virtual void CleanupSwapChain() const override;
	virtual bool IsDeviceSuitable(const VkPhysicalDevice device) override;
};


#endif
