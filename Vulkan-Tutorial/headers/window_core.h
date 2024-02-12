#ifndef WINDOW_CORE_H
#define WINDOW_CORE_H
#include <vulkan/vulkan_core.h>

namespace Trek
{
	class WindowCore
	{
	public:
		WindowCore() = default;
		virtual ~WindowCore() = default;
		virtual VkSurfaceKHR CreateVulkanSurface(const VkInstance& instance) const = 0;
	};
}

#endif
