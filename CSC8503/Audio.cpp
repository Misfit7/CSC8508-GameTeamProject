#include "Audio.h"

Audio::Audio(GameWorld* world)
{
    this->world = world;
    // start the sound engine with default parameters
    soundEngine = createIrrKlangDevice();
    if (!soundEngine)
    {
        printf("Could not startup soundEngine\n");
    }
    ISound* bgm = soundEngine->play3D((Assets::SOUNDSDIR + "getout.ogg").c_str(),
        vec3df(0, 0, 0), false, true);
}

Audio::~Audio()
{
    soundEngine->drop();
}

void Audio::UpdateKeys()
{
    Vector3 cp = world->GetMainCamera().GetPosition();
    vec3df position(cp.x, cp.y, cp.z);  // position of the listener
    vec3df lookDirection(10, 0, 10);    // the direction the listener looks into

    soundEngine->setListenerPosition(position, lookDirection, vec3df(0, 0, 0), vec3df(0, 1, 0));
    /*if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUMPAD9)) {
        playBGM = !playBGM;
    }*/
}
