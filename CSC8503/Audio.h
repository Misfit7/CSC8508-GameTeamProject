#pragma once
//irrKlang
//https://www.ambiera.com/irrklang/license.html
#include <stdio.h>
#include <irrKlang.h>
#include <conio.h>

#include "Window.h"
#include "Assets.h"
#include "GameWorld.h"

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

using namespace irrklang;
using namespace NCL;
using namespace CSC8503;

class Audio
{
public:
	Audio(GameWorld* world);
	~Audio();
	void Update();
	void PlayButton();
	void PlayGet();
	void PlayPut();
	void PlayFootstep();
	void PauseFootstep();
	void PlayWaterin();
	void PauseWaterin();
	void PlayWaterout();
	void PlayWood();
	void PauseWood();
	void PlayIron();
	void PauseIron();

	void PlayWin();
	void PlayFailure();

private:
	GameWorld* world;
	//Audio
	ISoundEngine* soundEngine = nullptr;

	ISound* menubgm = nullptr;
	ISound* playingbgm = nullptr;
	ISound* trainbgm = nullptr;
	ISound* button = nullptr;
	ISound* get = nullptr;
	ISound* put = nullptr;
	ISound* footstep = nullptr;
	ISound* wood = nullptr;
	ISound* iron = nullptr;
	ISound* waterin = nullptr;
	ISound* waterout = nullptr;

};
