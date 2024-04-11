#include "Window.h"
#include "Debug.h"

#include "TutorialGame.h"
#include "NetworkedGame.h"

#include <chrono>
#include <thread>
#include <sstream>

using namespace NCL;
using namespace CSC8503;

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead.

This time, we've added some extra functionality to the window class - we can
hide or show the

*/
int main() {
   // TestNetworking();
    Window* w = Window::CreateGameWindow("Train Bob", 1280, 720);
    if (!w->HasInitialised()) {
        return -1;
    }

    w->ShowOSPointer(false);
    w->LockMouseToWindow(true);
    NetworkedGame* g = new NetworkedGame();
    w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
    while (!g->IsExitGame() && w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyCodes::END)) {
        float dt = w->GetTimer().GetTimeDeltaSeconds();
        if (dt > 0.1f) {
            std::cout << "Skipping large time delta" << std::endl;
            continue; //must have hit a breakpoint or something to have a 1 second frame time!
        }

        if (Window::GetKeyboard()->KeyPressed(KeyCodes::PRIOR)) {
            w->ShowConsole(true);
        }
        if (Window::GetKeyboard()->KeyPressed(KeyCodes::NEXT)) {
            w->ShowConsole(false);
        }

        //if (Window::GetKeyboard()->KeyPressed(KeyCodes::T)) { 
        //    w->SetWindowPosition(0, 0);
        //}

        w->SetTitle("Train Bob");

        g->UpdateGame(dt);
    }

    Window::DestroyGameWindow();

}