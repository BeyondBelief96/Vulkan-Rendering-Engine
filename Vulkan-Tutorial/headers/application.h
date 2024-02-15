#ifndef APPLICATION_H
#define APPLICATION_H
#include "trek_window.h"
#include "trek_core.h"
#include "trek_renderer.h"

// std

#include "AeroWorld2D.h"

namespace Trek
{
	class Application
	{
	public:
		Application();
		~Application() = default;
		Application(const Application&) = delete;
		Application& operator=(Application&) = delete;
		Application(const Application&&) = delete;
		Application& operator=(Application&&) = delete;
		void run();
		static constexpr int WIDTH = 1000;
		static constexpr int HEIGHT = 800;
	private:
		TrekWindow trekWindow{WIDTH, HEIGHT, "Vulkan Tutorial!"};
		TrekCore trekDevice{ trekWindow };
		TrekRenderer trekRenderer{ trekWindow, trekDevice };
	};
}

#endif
