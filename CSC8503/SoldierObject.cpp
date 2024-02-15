#include "SoldierObject.h"
#include "Window.h"

void SoldierObject::Update(float dt) {
	if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM2)) {
		renderObject->SetFrameTime(renderObject->GetFrameTime() - dt);
		while (renderObject->GetFrameTime() < 0.0f) {
			renderObject->SetCurrentFrame((renderObject->GetCurrentFrame() + 1) %
				renderObject->GetAnim()->GetFrameCount());
			renderObject->SetFrameTime(renderObject->GetFrameTime() + 1.0f /
				renderObject->GetAnim()->GetFrameRate());
		}
	}
}