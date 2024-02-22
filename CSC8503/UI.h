#pragma once

#include "Window.h"
#include "Win32Window.h"
#include "../Plugins/imGui/imgui.h"
#include "../Plugins/imGui/imgui_impl_win32.h"
#include "../Plugins/imGui/imgui_impl_opengl3.h"

#include "Assets.h"
#include "GameWorld.h"

using namespace NCL;

class UI
{
public:
    UI(GameWorld* world);
    ~UI();
    void Update(float dt);
    void DrawUI();
    void ToggleShowUIdemo() { showUIdemo = !showUIdemo; };

protected:
    GameWorld* world;
    bool showUIdemo = true;
};
