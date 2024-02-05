#include "Audio.h"

using namespace std;

Audio::Audio(GameWorld* world)
{
    this->world = world;
    // start the sound engine with default parameters
    soundEngine = createIrrKlangDevice();
    if (!soundEngine)
    {
        printf("Could not startup soundEngine\n");
    }
    bgmsource = soundEngine->addSoundSourceFromFile((Assets::SOUNDSDIR + "getout.ogg").c_str());
}

Audio::~Audio()
{
    soundEngine->drop();
}

void Audio::UpdateKeys()
{
    if (playBGM && bgm == nullptr) {
        bgm = soundEngine->play3D(bgmsource, vec3df(1, 0, 1), true, false, true);
        bgm->setMinDistance(3.0f);
    }

    //paused bgm
    if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUMPAD9)) {
        playBGM = !playBGM;
    }
    if (playBGM)
    {
        bgm->setIsPaused(false);
    }
    else if (!playBGM) {
        bgm->setIsPaused(true);
    }

    Vector3 cp = world->GetMainCamera().GetPosition();
    //cout << cp << endl;
    vec3df position(cp.x, cp.y, cp.z);  // position of the listener
    Vector3 forward = Matrix4::Rotation(world->GetMainCamera().GetYaw(),
        Vector3(0, 1, 0)) * Matrix4::Rotation(world->GetMainCamera().GetPitch(), Vector3(1, 0, 0)) * Vector3(0, 0, -1);
    cout << forward << endl;
    vec3df lookDirection(-forward.x, forward.y, -forward.z);  // the direction the listener looks into

    soundEngine->setListenerPosition(position, lookDirection);

}
