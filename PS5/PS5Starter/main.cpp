//#include "AGCRenderer.h"
#include "PS5Window.h"
#include "../CSC8503/TutorialGame.h"
#include "../CSC8503CoreClasses/PhysicsSystem.h"
#include "../CSC8503CoreClasses/GameWorld.h"
#include "../CSC8503/GameTechRendererInterface.h"
#include "GameTechAGCRenderer.h"

using namespace NCL;
using namespace PS5;

size_t sceUserMainThreadStackSize = 2 * 1024 * 1024;
extern const char sceUserMainThreadName[] = "TeamProjectGameMain";
int sceUserMainThreadPriority = SCE_KERNEL_PRIO_FIFO_DEFAULT;
size_t sceLibcHeapSize = 256 * 1024 * 1024;

int main() {
	PS5Window* w = new PS5Window("Hello!", 1920, 1080);

	PS5Controller* c = w->GetController();

	c->MapAxis(0, "LeftX");
	c->MapAxis(1, "LeftY");

	c->MapAxis(2, "RightX");
	c->MapAxis(3, "RightY");

	c->MapAxis(4, "DX");
	c->MapAxis(5, "DY");

	c->MapButton(0, "Triangle");
	c->MapButton(1, "Circle");	
	c->MapButton(2, "Cross");
	c->MapButton(3, "Square");
	c->MapButton(4, "L2");
	c->MapButton(5, "R2");
	c->MapButton(6, "L1");
	c->MapButton(7, "R1");
	c->MapButton(8, "L3");
	c->MapButton(9, "R3");
	c->MapButton(10, "Options");

	//These are the axis/button aliases the inbuilt camera class reads from:
	c->MapAxis(0, "XLook");
	c->MapAxis(1, "YLook");

	c->MapAxis(2, "Sidestep");
	c->MapAxis(3, "Forward");

	c->MapButton(0, "Up");
	c->MapButton(2, "Down");


	GameWorld*		world	= new GameWorld();
	PhysicsSystem* physics	= new PhysicsSystem(*world);


#ifdef USEVULKAN
	GameTechVulkanRenderer* renderer = new GameTechVulkanRenderer(*world);
#elif USEAGC
	GameTechAGCRenderer* renderer = new GameTechAGCRenderer(*world);
#else 
	GameTechRenderer* renderer = new GameTechRenderer(*world);
#endif

	TutorialGame* g = new TutorialGame(*world , *renderer, *physics);

	while (w->UpdateWindow() && !g->IsExit()) {
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		g->UpdateGame(dt);
		renderer->Update(dt);
		renderer->Render();
		Debug::UpdateRenderables(dt);
	}

	delete physics;
	delete renderer;
	delete world;

	return 0;
}