#pragma once

class RenderLayer
{
public:
	virtual void onBegin() = 0;
	virtual void onEnd() = 0;
	virtual void onDraw() = 0;
	
	virtual ~RenderLayer() = default;
};