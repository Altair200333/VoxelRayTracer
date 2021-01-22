#include "Engine.h"
#include "SPATApp.h"

int main()
{
	Engine::init("Voxel ray tracer");
	SPATApp app;
	Engine::run();
	
	return 0;
}
