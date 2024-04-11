#include "Audio.h"
#include "TutorialGame.h"

using namespace std;

Audio::Audio(GameWorld* world)
{
	std::cout << std::endl << "--------Initialising Audio--------" << std::endl;

	this->world = world;
	// start the sound engine with default parameters
	soundEngine = createIrrKlangDevice();
	if (!soundEngine)
	{
		printf("Could not startup soundEngine\n");
	}
}

Audio::~Audio()
{
	soundEngine->drop();
}

void Audio::Update()
{
	Vector3 lp;
	Vector3 tp;
	vec3df lposition;
	vec3df tposition;
	Vector3 forward;
	vec3df lookDirection;
	switch (world->GetGameState())
	{
	case GameState::LOADING:
	case GameState::MENU:
		soundEngine->setAllSoundsPaused(false);
		if (menubgm == nullptr) {
			soundEngine->stopAllSounds();
			if (playingbgm != nullptr)
			{
				playingbgm->drop();
				playingbgm = nullptr;
				trainbgm->drop();
				trainbgm = nullptr;
			}
			menubgm = soundEngine->play2D((Assets::SOUNDSDIR + "Menu.mp3").c_str(), true, false, true);
			//menubgm->setVolume(0.075f);
		}
		break;
	case GameState::PLAYING:
	case GameState::SERVERPLAYING:
	case GameState::CLIENTPLAYING:
		lp = TutorialGame::GetGame()->GetPlayer()->GetTransform().GetPosition();
		lposition = vec3df(lp.x, lp.y, lp.z);  // position of the listener
		forward = TutorialGame::GetGame()->GetPlayer()->GetFace();
		//Vector3 forward = Matrix4::Rotation(world->GetMainCamera().GetYaw(),
		//    Vector3(0, 1, 0)) * Matrix4::Rotation(world->GetMainCamera().GetPitch(), Vector3(1, 0, 0)) * Vector3(0, 0, -1);
		lookDirection = vec3df(-forward.x, forward.y, -forward.z);  // the direction the listener looks into
		soundEngine->setAllSoundsPaused(false);
		if (menubgm != nullptr) {
			soundEngine->stopAllSounds();
			menubgm->drop();
			menubgm = nullptr;
			if (playingbgm == nullptr)
			{
				playingbgm = soundEngine->play2D((Assets::SOUNDSDIR + "PlayingBGM.mp3").c_str(), true, false, true);
				//playingbgm->setVolume(0.05f);
				trainbgm = soundEngine->play3D((Assets::SOUNDSDIR + "train.mp3").c_str(), vec3df(1, 0, 1), true, false, true);
				//trainbgm->setVolume(0.5f);
			}
		}
		tp = TutorialGame::GetGame()->GetTrain()->GetTransform().GetPosition();
		tposition = vec3df(tp.x, tp.y, tp.z);
		trainbgm->setPosition(tposition);
		soundEngine->setListenerPosition(lposition, lookDirection);
		break;
	case GameState::PAUSED:
		soundEngine->setAllSoundsPaused(true);
		break;
	default:
		break;
	}

}

void Audio::PlayButton()
{
	button = soundEngine->play2D((Assets::SOUNDSDIR + "Button.mp3").c_str(), false, false, true);
	//button->setVolume(0.25);
}

void Audio::PlayGet()
{
	soundEngine->play2D((Assets::SOUNDSDIR + "get.wav").c_str(), false, false, false);
}

void Audio::PlayPut()
{
	soundEngine->play2D((Assets::SOUNDSDIR + "put.wav").c_str(), false, false, false);
}

void Audio::PlayFootstep()
{
	if (footstep == nullptr)
	{
		footstep = soundEngine->play2D((Assets::SOUNDSDIR + "footstep.mp3").c_str(), true, false, true);
		//footstep->setVolume(0.75);
	}
	footstep->setIsPaused(false);
}

void Audio::PauseFootstep()
{
	if (footstep != nullptr)
		footstep->setIsPaused(true);
}

void Audio::PlayWaterin()
{
	if (waterin == nullptr)
	{
		waterin = soundEngine->play2D((Assets::SOUNDSDIR + "waterin.wav").c_str(), true, false, true);
	}
	waterin->setIsPaused(false);
}

void Audio::PauseWaterin()
{
	if (waterin != nullptr)
		waterin->setIsPaused(true);
}


void Audio::PlayWaterout()
{
	soundEngine->play2D((Assets::SOUNDSDIR + "waterout.wav").c_str(), false, false, false);
}

void Audio::PlayWood()
{
	if (wood == nullptr)
	{
		wood = soundEngine->play2D((Assets::SOUNDSDIR + "wood.mp3").c_str(), true, false, true);
		//wood->setVolume(0.75);
	}
	wood->setIsPaused(false);
}

void Audio::PauseWood()
{
	if (wood != nullptr)
		wood->setIsPaused(true);
}

void Audio::PlayIron()
{
	if (iron == nullptr)
	{
		iron = soundEngine->play2D((Assets::SOUNDSDIR + "iron.mp3").c_str(), true, false, true);
		//iron->setVolume(0.75);
	}
	iron->setIsPaused(false);
}

void Audio::PauseIron()
{
	if (iron != nullptr)
		iron->setIsPaused(true);
}

void Audio::PlayWin()
{
	soundEngine->play2D((Assets::SOUNDSDIR + "win.wav").c_str(), false, false, false);
}

void Audio::PlayFailure()
{
	soundEngine->play2D((Assets::SOUNDSDIR + "failure.mp3").c_str(), false, false, false);
}
