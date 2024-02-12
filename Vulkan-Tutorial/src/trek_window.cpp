#include "trek_window.h"

namespace Trek
{
	TrekWindow::TrekWindow(int width, int height, const std::string& name)
	: m_width(width), m_height(height), m_windowName(name)
	{
		InitWindow();
	}

	void TrekWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}
}
