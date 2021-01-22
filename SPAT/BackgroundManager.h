#pragma once
#include "Background.h"
#include "GuiDrawable.h"
#include "vector"
#include "ImGui/imgui.h"

class BackgroundManager final: public GuiDrawable
{
	int currentItem = 0;
public:
	std::vector<std::pair<std::string, std::shared_ptr<Background>>> backgrounds;
	
	void addBackground(const std::string& name, const std::shared_ptr<Background>& background)
	{
		backgrounds.emplace_back(name, background);
	}

	[[nodiscard]] std::shared_ptr<Background> getActiveBackground() const
	{
		return backgrounds[currentItem].second;
	}
	
	void onGuiDraw() override
	{
		std::string names;
		for (auto && [name, back] : backgrounds)
		{
			names += name + '\0';
		}
		
		ImGui::Combo("Renderer", &currentItem, reinterpret_cast<const char*>(&names[0]), backgrounds.size());
		
		if(getActiveBackground())
		{
			getActiveBackground()->onGuiDraw();
		}
	}
};
