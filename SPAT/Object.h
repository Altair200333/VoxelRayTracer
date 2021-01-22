#pragma once
#include "ComponentManager.h"

class Object final
{
	ComponentManager componentManager;
public:
	template<typename T>
	void addComponent(std::shared_ptr<T> component)
	{
		componentManager.addComponent(component, this);
	}
	
	template<typename T>
	auto getComponent() const
	{
		return componentManager.getComponent<T>();
	}
	
};
