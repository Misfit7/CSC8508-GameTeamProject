#include "TutorialGame.h"
#include "PlayerObject.h"
#include "NetworkedGame.h"
#include "NavigationGrid.h"
#include "Window.h"

using namespace NCL::CSC8503;

void PlayerObject::Update(float dt) {
    if (NetworkedGame::GetNetworkedGame()->IsServer() || !TutorialGame::GetGame()->IsNetworked()) {

        if (renderObject->GetAnimationObject())
            UpdateAnimation(dt);
        if (!TutorialGame::GetGame()->IsNetworked() ||
            (TutorialGame::GetGame()->IsNetworked() && (TutorialGame::GetGame()->GetWorld()->GetGameState() == SERVERPLAYING || TutorialGame::GetGame()->GetWorld()->GetGameState() == CLIENTPLAYING)))
            PlayerMovement(dt);

        doing = false;
        cutting = false;
        digging = false;

        showDebug = TutorialGame::GetGame()->ShowDebug();

        CutTree();
        DigRock();
        ScoopWater();
        UseWater();
        BuildBridge();
        LoadMaterial();
        UseRobot();
        RunFast(dt);

        //Vector3 position = transform.GetPosition();
        //Vector3 p = FindGrid(Vector3(position.x, 2, position.z));
        //int index = p.x / 10 + (p.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
        //std::cout << "Index: " << index << std::endl;
        //std::cout << "Type: " << TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index).type << std::endl;
    }
}

void PlayerObject::OnCollisionBegin(GameObject* otherObject) {

}
static float a = 0;

void PlayerObject::UpdateAnimation(float dt) {
    bool WHeld = false;
    bool DHeld = false;
    bool AHeld = false;
    bool SHeld = false;

    if (networkObject->GetNetworkID() == 1) {
        WHeld = Window::GetKeyboard()->KeyHeld(KeyCodes::W);
        DHeld = Window::GetKeyboard()->KeyHeld(KeyCodes::D);
        AHeld = Window::GetKeyboard()->KeyHeld(KeyCodes::A);
        SHeld = Window::GetKeyboard()->KeyHeld(KeyCodes::S);
    }
    else {
        WHeld = buttonStates[0];
        SHeld = buttonStates[1];
        AHeld = buttonStates[2];
        DHeld = buttonStates[3];
    }

    if (!doing) {
        if (WHeld && renderObject->GetAnimationObject()->GetAnim2()) {
            if (renderObject->GetAnimationObject()->GetFlag2()) {
                renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim2());
                renderObject->GetAnimationObject()->SetCurrentFrame(0);
                renderObject->GetAnimationObject()->SetFrameTime(0.0f);
                renderObject->GetAnimationObject()->UpdateFlags(2);
            }
        }
        else if (DHeld && renderObject->GetAnimationObject()->GetAnim3()) {
            if (renderObject->GetAnimationObject()->GetFlag3()) {
                renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim3());
                renderObject->GetAnimationObject()->SetCurrentFrame(0);
                renderObject->GetAnimationObject()->SetFrameTime(0.0f);
                renderObject->GetAnimationObject()->UpdateFlags(3);
            }
        }
        else if (AHeld && renderObject->GetAnimationObject()->GetAnim4()) {
            if (renderObject->GetAnimationObject()->GetFlag4()) {
                renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim4());
                renderObject->GetAnimationObject()->SetCurrentFrame(0);
                renderObject->GetAnimationObject()->SetFrameTime(0.0f);
                renderObject->GetAnimationObject()->UpdateFlags(4);
            }
        }
        else if (SHeld && renderObject->GetAnimationObject()->GetAnim5()) {
            if (renderObject->GetAnimationObject()->GetFlag5()) {
                renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim5());
                renderObject->GetAnimationObject()->SetCurrentFrame(0);
                renderObject->GetAnimationObject()->SetFrameTime(0.0f);
                renderObject->GetAnimationObject()->UpdateFlags(5);
            }
        }
        else if (Window::GetKeyboard()->KeyHeld(KeyCodes::NUM3) && renderObject->GetAnimationObject()->GetAnim8()) {
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
    }

    else {
        if (cutting && renderObject->GetAnimationObject()->GetAnim6()) {
            if (renderObject->GetAnimationObject()->GetFlag6()) {
                renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim6());
                renderObject->GetAnimationObject()->SetCurrentFrame(0);
                renderObject->GetAnimationObject()->SetFrameTime(0.0f);
                renderObject->GetAnimationObject()->UpdateFlags(6);
            }
        }
        else if (digging && renderObject->GetAnimationObject()->GetAnim7()) {
            if (renderObject->GetAnimationObject()->GetFlag7()) {
                renderObject->GetAnimationObject()->SetActiveAnim(renderObject->GetAnimationObject()->GetAnim7());
                renderObject->GetAnimationObject()->SetCurrentFrame(0);
                renderObject->GetAnimationObject()->SetFrameTime(0.0f);
                renderObject->GetAnimationObject()->UpdateFlags(7);
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
}

void PlayerObject::PlayerMovement(float dt) {
    bool WHeld = false;
    bool DHeld = false;
    bool AHeld = false;
    bool SHeld = false;

    if (networkObject->GetNetworkID() == 1) {
        WHeld = Window::GetKeyboard()->KeyHeld(KeyCodes::W);
        DHeld = Window::GetKeyboard()->KeyHeld(KeyCodes::D);
        AHeld = Window::GetKeyboard()->KeyHeld(KeyCodes::A);
        SHeld = Window::GetKeyboard()->KeyHeld(KeyCodes::S);
    }
    else {
        WHeld = buttonStates[0];
        SHeld = buttonStates[1];
        AHeld = buttonStates[2];
        DHeld = buttonStates[3];
    }

    Quaternion* qq;
    if(!running)speed = TutorialGame::GetGame()->IsNetworked() ? 50 : 50;
    //float yaw = Maths::RadiansToDegrees(atan2(-np.x, -np.z));
    //start->GetTransform().SetOrientation(qq->EulerAnglesToQuaternion(0, yaw, 0));

    if (WHeld) {
        face = Vector3(0, 0, -1);
        physicsObject->SetRealDamping(0.962f);
        physicsObject->AddForceAtPosition(face * speed, transform.GetPosition());
        transform.SetOrientation(qq->EulerAnglesToQuaternion(0, 0, 0));
        TutorialGame::GetGame()->GetAudio()->PlayFootstep();
    }
    else if (AHeld) {
        face = Vector3(-1, 0, 0);
        physicsObject->SetRealDamping(0.962f);
        physicsObject->AddForceAtPosition(face * speed, transform.GetPosition());
        transform.SetOrientation(qq->EulerAnglesToQuaternion(0, 90, 0));
        TutorialGame::GetGame()->GetAudio()->PlayFootstep();
    }
    else if (SHeld) {
        face = Vector3(0, 0, 1);
        physicsObject->SetRealDamping(0.962f);
        physicsObject->AddForceAtPosition(face * speed, transform.GetPosition());
        transform.SetOrientation(qq->EulerAnglesToQuaternion(0, 180, 0));
        TutorialGame::GetGame()->GetAudio()->PlayFootstep();
    }
    else if (DHeld) {
        face = Vector3(1, 0, 0);
        physicsObject->SetRealDamping(0.962f);
        physicsObject->AddForceAtPosition(face * speed, transform.GetPosition());
        transform.SetOrientation(qq->EulerAnglesToQuaternion(0, -90, 0));
        TutorialGame::GetGame()->GetAudio()->PlayFootstep();
    }
    else {
        physicsObject->SetRealDamping(0.858f);
        TutorialGame::GetGame()->GetAudio()->PauseFootstep();
    }
    //std::cout << "Player: " << transform.GetPosition().x << " " << transform.GetPosition().y << " " << transform.GetPosition().z << std::endl;

//else {
//   // physicsObject->SetLinearVelocity(Vector3(0, 0, 0));
//}
//a += dt;
//if (a >= 1.0f) {
//    std::cout << "The force is: " << physicsObject->GetLinearVelocity().x << " " << physicsObject->GetLinearVelocity().y << " " << physicsObject->GetLinearVelocity().z << std::endl;
//    a = 0;
//}
//std::cout << "Player: " << transform.GetPosition().x << " " << transform.GetPosition().y << " " << transform.GetPosition().z << std::endl;
}

void PlayerObject::UpdateFace() {
    if (transform.GetOrientation().ToEuler() == Vector3(0, 0, 0))
        face = Vector3(0, 0, -1);
    else if (transform.GetOrientation().ToEuler() == Vector3(0, 90, 0))
        face = Vector3(-1, 0, 0);
    else if (transform.GetOrientation().ToEuler() == Vector3(0, 180, 0))
        face = Vector3(0, 0, 1);
    else if (transform.GetOrientation().ToEuler() == Vector3(0, -90, 0))
        face = Vector3(1, 0, 0);
}

//void PlayerObject::SwitchSkin() {
//    if (Window::GetKeyboard()->KeyPressed(KeyCodes::E)) {
//        renderObject->SetMesh(meshes[(index + 1) % 5]);
//        renderObject->SetAnimationObject(animations[(index + 1) % 5]);
//        renderObject->SetTextures(textures[(index + 1) % 5]);
//        renderObject->SetBumpTextures(bumpTextures[(index + 1) % 5]);
//        renderObject->SetShaderGroup(shaders[(index + 1) % 5]);
//        index++;
//    }
//}

void PlayerObject::CutTree() {
    bool spaceHeld = false;
    if (networkObject->GetNetworkID() == 1)
        spaceHeld = Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::SPACE);
    else
        spaceHeld = buttonStates[5];

    if (spaceHeld && slot == 3) {
        int worldID1;
        doing = true;
        cutting = true;
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld()->Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 10086 && closestCollision.rayDistance < range) {
                TutorialGame::GetGame()->GetAudio()->PlayWood();
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 1, 0, 1));
                closest->SetFlag1(true);
                closest->GetTransform().SetScale(closest->GetTransform().GetScale() - Vector3(0.05, 0.05, 0.05));
                if (closest->GetTransform().GetScale().x < 0.1f) {
                    Vector3 position = FindGrid(closest->GetTransform().GetPosition());
                    int index = position.x / 10 + (position.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
                    GridNode& n = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index);
                    n.SetType(0);
                    closest->SetFlag1(false);
                    if (TutorialGame::GetGame()->IsNetworked()) {
                        worldID1 = closest->GetWorldID();
                    }
                    PlankObject* plank1 = TutorialGame::GetGame()->AddPlankToWorld(Vector3(closest->GetTransform().GetPosition().x, 5, closest->GetTransform().GetPosition().z));
                    TutorialGame::GetGame()->GetWorld()->RemoveGameObject(closest, false);
                    if (TutorialGame::GetGame()->IsNetworked()) {
                        NetworkedGame::GetNetworkedGame()->SetCutTreeFlag(true);
                        NetworkedGame::GetNetworkedGame()->SetPlankNetworkID(plank1->GetNetworkObject()->GetNetworkID());
                        NetworkedGame::GetNetworkedGame()->SetTreeWorldID(worldID1);
                        NetworkedGame::GetNetworkedGame()->SetTreeCutTag(1);
                    }
                }
            }
            else {
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
            }
        }
        else {
            if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 1, 1, 1));
        }
    }
    else
    {
        TutorialGame::GetGame()->GetAudio()->PauseWood();
    }
}

void PlayerObject::DigRock() {
    bool spaceHeld = false;
    if (networkObject->GetNetworkID() == 1)
        spaceHeld = Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::SPACE);
    else
        spaceHeld = buttonStates[5];

    if (spaceHeld && slot == 2) {
        int worldID1;
        doing = true;
        digging = true;
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld()->Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 10010 && closestCollision.rayDistance < range) {
                TutorialGame::GetGame()->GetAudio()->PlayIron();
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 1, 0, 1));
                closest->SetFlag1(true);
                closest->GetTransform().SetScale(closest->GetTransform().GetScale() - Vector3(0.05, 0.05, 0.05));
                if (closest->GetTransform().GetScale().x < 0.1f) {
                    Vector3 position = FindGrid(closest->GetTransform().GetPosition());
                    int index = position.x / 10 + (position.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
                    GridNode& n = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index);
                    n.SetType(0);
                    closest->SetFlag1(false);
                    if (TutorialGame::GetGame()->IsNetworked()) {
                        worldID1 = closest->GetWorldID();
                    }
                    StoneObject* stone1 = TutorialGame::GetGame()->AddStoneToWorld(Vector3(closest->GetTransform().GetPosition().x, 5, closest->GetTransform().GetPosition().z));
                    TutorialGame::GetGame()->GetWorld()->RemoveGameObject(closest, false);
                    if (TutorialGame::GetGame()->IsNetworked()) {
                        NetworkedGame::GetNetworkedGame()->SetDigRockFlag(true);
                        NetworkedGame::GetNetworkedGame()->SetStoneNetworkID(stone1->GetNetworkObject()->GetNetworkID());
                        NetworkedGame::GetNetworkedGame()->SetRockWorldID(worldID1);
                        NetworkedGame::GetNetworkedGame()->SetRockDugTag(2);
                    }
                }
            }
            else {
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
            }
        }
        else {
            if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
        }
    }
    else
    {
        TutorialGame::GetGame()->GetAudio()->PauseIron();
    }
}

void PlayerObject::ScoopWater() {
    bool spaceHeld = false;
    if (networkObject->GetNetworkID() == 1)
        spaceHeld = Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::SPACE);
    else
        spaceHeld = buttonStates[5];

    if (spaceHeld && slot == 4) {
        doing = true;
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld()->Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 10000 && closestCollision.rayDistance < range) {
                TutorialGame::GetGame()->GetAudio()->PlayWaterin();
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 1, 0, 1));
                TutorialGame::GetGame()->GetBucket()->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
                TutorialGame::GetGame()->GetBucket()->SetWater(true);
                TutorialGame::GetGame()->GetBucket()->SetFlag1(true);
            }
            else {
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
            }
        }
        else {
            if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
        }
    }
    else
    {
        TutorialGame::GetGame()->GetAudio()->PauseWaterin();
    }
}

void PlayerObject::UseWater() {
    bool spaceHeld = false;
    if (networkObject->GetNetworkID() == 1)
        spaceHeld = Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::SPACE);
    else
        spaceHeld = buttonStates[5];

    if (spaceHeld && slot == 4) {
        doing = true;
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld()->Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 23 && closestCollision.rayDistance < range && TutorialGame::GetGame()->GetBucket()->GetWater() == true) {
                TutorialGame::GetGame()->GetAudio()->PlayWaterout();
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 1, 0, 1));
                WaterCarriage* waterCarriage = (WaterCarriage*)closest;
                waterCarriage->SetCarriageWater(100.0f);
                TutorialGame::GetGame()->GetTrain()->SetOnFire(false);
                TutorialGame::GetGame()->GetTrain()->SetFire(100.0f);
                TutorialGame::GetGame()->GetBucket()->SetWater(false);
                TutorialGame::GetGame()->GetBucket()->SetFlag1(false);
                TutorialGame::GetGame()->GetBucket()->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
            }
            else {
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
            }
        }
        else {
            if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
        }
    }
}

void PlayerObject::BuildBridge() {
    bool FPressed = false;
    if (networkObject->GetNetworkID() == 1)
        FPressed = Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::F);
    else
        FPressed = buttonStates[7];

    if (FPressed && slot == 5) {
        int worldID1;
        doing = true;
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld()->Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 10000 && closestCollision.rayDistance < range) {
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 1, 0, 1));
                bridgePosition = closest->GetTransform().GetPosition();
                if (TutorialGame::GetGame()->IsNetworked()) {
                    worldID1 = closest->GetWorldID();
                }
                TutorialGame::GetGame()->GetWorld()->RemoveGameObject(closest, false);
                building = true;
                if (TutorialGame::GetGame()->IsNetworked()) {
                    NetworkedGame::GetNetworkedGame()->SetBuildBridgeFlag(true);
                    NetworkedGame::GetNetworkedGame()->SetBridgeBuiltTag(8);
                    NetworkedGame::GetNetworkedGame()->SetWaterWorldID(worldID1);
                }
            }
            else {
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
            }
        }
        else {
            if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
        }
    }
}

void PlayerObject::LoadMaterial() {
    bool spacePressed = false;
    if (networkObject->GetNetworkID() == 1)
        spacePressed = Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::SPACE);
    else
        spacePressed = buttonStates[6];

    if (spacePressed) {
        if (slot == 5 || slot == 6) {
            Ray r = Ray(transform.GetPosition(), face);
            RayCollision closestCollision;
            if (TutorialGame::GetGame()->GetWorld()->Raycast(r, closestCollision, true, this)) {
                GameObject* closest = (GameObject*)closestCollision.node;
                if (closest->GetTypeID() == 21 && closestCollision.rayDistance < range) {
                    TutorialGame::GetGame()->GetAudio()->PlayGet();
                    if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 1, 0, 1));
                    carriage = (MaterialCarriage*)closest;
                }
                else {
                    if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
                }
            }
            else {
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
            }
        }
    }
}

bool PlayerObject::CanPlaceRail() {
    bool canConnect = false;
    bool isPath = false;
    bool notRail = false;
    int connectedIndex;
    Vector3 position = FindGrid(Vector3(transform.GetPosition().x, 4.5f, transform.GetPosition().z));
    int index = position.x / 10 + (position.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
    GridNode& n = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index);
    notRail = n.type != 7 ? true : false;
    for (int i = 0; i < 4; ++i) {
        if (n.connected[i]) {
            if (n.connected[i]->type == 7) {
                canConnect = true;
                connectedIndex = i;
                break;
            }
        }
    }
    if ((TutorialGame::GetGame()->GetTrain()->GetLastPath() - Vector3(0, 3.5f, 0) - position).Length() < 14)
        isPath = true;

    return canConnect && isPath && notRail;
}

void PlayerObject::UseRobot() {
    if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::F)) {
        if (slot == 2 || slot == 3) {
            Ray r = Ray(transform.GetPosition(), face);
            RayCollision closestCollision;
            if (TutorialGame::GetGame()->GetWorld()->Raycast(r, closestCollision, true, this)) {
                GameObject* closest = (GameObject*)closestCollision.node;
                if (closest->GetTypeID() == 11 && closestCollision.rayDistance < range) {
                    TutorialGame::GetGame()->GetAudio()->PlayGet();
                    if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 1, 0, 1));
                    TutorialGame::GetGame()->GetRobot()->SetPlayer(this);
                    if (slot == 2) robotDig = true;
                    if (slot == 3) robotCut = true;
                }
                else {
                    if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
                }
            }
            else {
                if (TutorialGame::GetGame()->ShowDebug())Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + face * 5.0f, Vector4(1, 0, 0, 1));
            }
        }
    }
}

void PlayerObject::RunFast(float dt) {
    bool shiftPressed = false;
    if (networkObject->GetNetworkID() == 1)
        shiftPressed = Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::SHIFT);
    else
        shiftPressed = buttonStates[8];

    if (coolDown < 2.0f) {
        coolDown += dt;
    }
    if (shiftPressed && coolDown>=2.0f) {
        running = true;
        coolDown = 0.0f;
        runPower = 0.11f;
    }
    if (running) {
        if (runPower <= 0) {
            running = false;
            speed = TutorialGame::GetGame()->IsNetworked() ? 50 : 50;
            physicsObject->SetRealDamping(0.962);
        }
        if (runPower > 0 && runPower < 0.05f) {
            speed = TutorialGame::GetGame()->IsNetworked() ? 50 : 50;
            physicsObject->SetRealDamping(0.825);
        }
        else {
            speed = TutorialGame::GetGame()->IsNetworked() ? 2000 : 2000; 
        }
        runPower -= dt;
    }
}
