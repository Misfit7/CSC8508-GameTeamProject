#pragma once
//irrKlang
#include <stdio.h>
#include <irrKlang.h>
#include <conio.h>

#include "Window.h"
#include "Assets.h"
#include "GameWorld.h"

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

using namespace irrklang;
using namespace NCL;

class Audio
{
public:
    Audio(GameWorld* world);
    ~Audio();
    void UpdateKeys();

private:
    GameWorld* world;
    //Audio
    ISoundEngine* soundEngine = nullptr;

    ISound* bgm = nullptr;
    bool playBGM = true;
};
