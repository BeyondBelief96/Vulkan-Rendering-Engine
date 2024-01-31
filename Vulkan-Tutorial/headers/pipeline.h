#ifndef PIPELINE_H
#define PIPELINE_H
#include <vector>
#include <vulkan/vulkan_core.h>

const std::vector<VkDynamicState> DYNAMIC_STATES = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
};

#endif
