#ifndef VULKAN_CORE_H
#define VULKAN_CORE_H
#include <optional>
#include <set>
#include <vulkan/vulkan_core.h>

#include "glfw_core.h"
#include "swap_chain.h"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	bool IsComplete() const
	{
		return graphics_family.has_value() && present_family.has_value();
	}
};

static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR& surface)
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
            indices.present_family = i;
        }
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = i;
        }

        if (indices.IsComplete()) break;

        i++;
    }
    return indices;
}

static bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

class VulkanCore
{
public:
	VulkanCore() = default;
	VulkanCore(VulkanCore& core) = default;
	~VulkanCore() = default;

	void Init(WindowCore* surfaceCreator);
	void Cleanup() const;

	VkInstance& GetInstance() { return m_instance; }
    VkSurfaceKHR& GetSurface() { return m_surface; }
    VkPhysicalDevice& GetPhysicalDevice() { return m_physicalDevice; }
    QueueFamilyIndices& GetQueueFamilies() { return m_indices; }

    uint32_t FindBufferMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

private:
	void CreateInstance();
	void SetupDebugMessenger();
	bool IsDeviceSuitable(const VkPhysicalDevice device);
	void PickPhysicalDevice();

	WindowCore* m_surfaceCreator = VK_NULL_HANDLE;
	VkInstance m_instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	QueueFamilyIndices m_indices;
};

#endif
