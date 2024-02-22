#ifndef TREK_FRAME_INFO
#define TREK_FRAME_INFO

#include "trek_camera.h"

//lib
#include <vulkan/vulkan.h>

namespace Trek
{
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		TrekCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}

#endif
