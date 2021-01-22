#pragma once
#include <string>
#include <utility>
#include "EventDispatcher.h"
#include "OnUpdateEvent.h"
#include "RenderLayersSet.h"
#include <memory>

class Window;
class Engine final
{
	std::shared_ptr<Window> window;
	RenderLayersSet renderLayers;

	static Engine& instance();

public:
	static std::shared_ptr<Window> getWindow();

	static void init(const std::string& windowName);
	static void run();

	static void addRenderLayer(std::shared_ptr<RenderLayer> layer);
};

#include "Window.h"

inline Engine& Engine::instance()
{
	static Engine engine;
	return engine;
}

inline std::shared_ptr<Window> Engine::getWindow()
{
	return instance().window;
}

inline void Engine::init(const std::string& windowName)
{
	instance().window = std::make_shared<Window>(windowName);
}

inline void Engine::run()
{
	Logger::log("Engine running");
	
	while (instance().window->running)
	{
		instance().window->clear();
		
		instance().renderLayers.render();

		EventDispatcher::dispatch(OnUpdateEvent());		
	}
}

inline void Engine::addRenderLayer(std::shared_ptr<RenderLayer> layer)
{
	instance().renderLayers.add(std::move(layer));
}

