#include "TutorialGame.h"
#include "PlayerObject.h"
#include "Window.h"

using namespace NCL::CSC8503;

void PlayerObject::Update(float dt) {
    if (Window::GetKeyboard()->KeyDown(KeyCodes::UP) && renderObject->GetAnimationObject()->GetAnim2()) {
        if (renderObject->GetAnimationObject()->GetFlag2()) {
            renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim2());
            renderObject->GetAnimationObject()->SetCurrentFrame(0);
            renderObject->GetAnimationObject()->SetFrameTime(0.0f);
            renderObject->GetAnimationObject()->UpdateFlags(2);
        }
    }
    else if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT) && renderObject->GetAnimationObject()->GetAnim3()) {
        if (renderObject->GetAnimationObject()->GetFlag3()) {
            renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim3());
            renderObject->GetAnimationObject()->SetCurrentFrame(0);
            renderObject->GetAnimationObject()->SetFrameTime(0.0f);
            renderObject->GetAnimationObject()->UpdateFlags(3);
        }
    }
    else if (Window::GetKeyboard()->KeyDown(KeyCodes::LEFT) && renderObject->GetAnimationObject()->GetAnim4()) {
        if (renderObject->GetAnimationObject()->GetFlag4()) {
            renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim4());
            renderObject->GetAnimationObject()->SetCurrentFrame(0);
            renderObject->GetAnimationObject()->SetFrameTime(0.0f);
            renderObject->GetAnimationObject()->UpdateFlags(4);
        }
    }
    else if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN) && renderObject->GetAnimationObject()->GetAnim5()) {
        if (renderObject->GetAnimationObject()->GetFlag5()) {
            renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim5());
            renderObject->GetAnimationObject()->SetCurrentFrame(0);
            renderObject->GetAnimationObject()->SetFrameTime(0.0f);
            renderObject->GetAnimationObject()->UpdateFlags(5);
        }
    }
    else if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM1) && renderObject->GetAnimationObject()->GetAnim6()) {
        if (renderObject->GetAnimationObject()->GetFlag6()) {
            renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim6());
            renderObject->GetAnimationObject()->SetCurrentFrame(0);
            renderObject->GetAnimationObject()->SetFrameTime(0.0f);
            renderObject->GetAnimationObject()->UpdateFlags(6);
        }
    }
    else if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM2) && renderObject->GetAnimationObject()->GetAnim7()) {
        if (renderObject->GetAnimationObject()->GetFlag7()) {
            renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim7());
            renderObject->GetAnimationObject()->SetCurrentFrame(0);
            renderObject->GetAnimationObject()->SetFrameTime(0.0f);
            renderObject->GetAnimationObject()->UpdateFlags(7);
        }
    }
    else if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM3) && renderObject->GetAnimationObject()->GetAnim8()) {
        if (renderObject->GetAnimationObject()->GetFlag8()) {
            renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim8());
            renderObject->GetAnimationObject()->SetCurrentFrame(0);
            renderObject->GetAnimationObject()->SetFrameTime(0.0f);
            renderObject->GetAnimationObject()->UpdateFlags(8);
        }
    }
    else {
        if (renderObject->GetAnimationObject()->GetFlag1()) {
            renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim1());
            renderObject->GetAnimationObject()->SetCurrentFrame(0);
            renderObject->GetAnimationObject()->SetFrameTime(0.0f);
            renderObject->GetAnimationObject()->UpdateFlags(1);
        }
    }

    if (renderObject->GetAnimationObject()->GetActiveAnim() != renderObject->GetAnimationObject()->GetAnim1() ||
        renderObject->GetAnimationObject()->HasIdle()) {
        renderObject->GetAnimationObject()->SetFrameTime(renderObject->GetAnimationObject()->GetFrameTime() - dt);
        while (renderObject->GetAnimationObject()->GetFrameTime() < 0.0f) {
            renderObject->GetAnimationObject()->SetCurrentFrame((renderObject->GetAnimationObject()->GetCurrentFrame() + 1) %
                renderObject->GetAnimationObject()->GetActiveAnim()->GetFrameCount());
            renderObject->GetAnimationObject()->SetFrameTime(renderObject->GetAnimationObject()->GetFrameTime() + 1.0f /
                renderObject->GetAnimationObject()->GetActiveAnim()->GetFrameRate());
        }
    }

    //if (Window::GetKeyboard()->KeyPressed(KeyCodes::E)) {
    //    renderObject->SetMesh(meshes[(index + 1) % 5]);
    //    renderObject->SetAnimationObject(animations[(index + 1) % 5]);
    //    renderObject->SetTextures(textures[(index + 1) % 5]);
    //    renderObject->SetBumpTextures(bumpTextures[(index + 1) % 5]);
    //    renderObject->SetShaderGroup(shaders[(index + 1) % 5]);
    //    index++;
    //}
}