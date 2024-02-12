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
		TrekWindow(int width, int height, const std::string& name);
		~TrekWindow();
	private:

		void InitWindow();

		const int m_width;
		const int m_height;
		GLFWwindow* m_window;
		std::string m_windowName;
	};
}

#endif
