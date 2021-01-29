#pragma once
#include "Application.h"
#include "GuiSubscriber.h"
#include "Scene.h"
#include "ViewportRenderer.h"

class SPATApp final: public Application, virtual public GuiSubscriber
{
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	ViewportRenderer level;
	
public:
	explicit SPATApp();

	void onUpdate() override;

	void onGuiDraw() override;

private:
	void openFile(); 
};
