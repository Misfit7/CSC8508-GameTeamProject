#include "WaterObject.h"
#include "RenderObject.h"

using namespace NCL::CSC8503;

void WaterObject::Update(float dt) {
	renderObject->SetTextureRotate(renderObject->GetTextureRotate() + dt * 2.0f);
	renderObject->SetTextureCycle(renderObject->GetTextureCycle() + dt * 0.25f);
}