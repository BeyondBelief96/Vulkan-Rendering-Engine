#ifndef TREK_WINDOW_H
#define TREK_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Trek
{
	class TrekWindow
	{
	public:
		TrekWindow(int width, int height, std::string name);
		~TrekWindow();
		TrekWindow(const TrekWindow&) = delete;
		TrekWindow& operator=(const TrekWindow&) = delete;

		bool shouldClose() const{ return glfwWindowShouldClose(window);}
		VkExtent2D getExtent() const { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const;
		GLFWwindow* getWindow() const { return window; }
		int getWindowWidth() const { return width; }
		int getWindowHeight() const { return height; }
		bool wasWindowResized() const { return frameBufferResized; }
		void resetWindowResizedFlag() { frameBufferResized = false; }
	private:

		static void framebufferResizedCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool frameBufferResized = false;
		GLFWwindow* window;
		std::string windowName;
	};
}

#endif
