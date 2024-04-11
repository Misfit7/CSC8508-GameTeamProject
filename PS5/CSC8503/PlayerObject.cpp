#include "TutorialGame.h"
#include "PlayerObject.h"
#include "NavigationGrid.h"
#include "Window.h"

#include <stdlib.h>

using namespace NCL::CSC8503;

void PlayerObject::Update(float dt) {
        PlayerMovement(dt);

        doing = false;
        cutting = false;
        digging = false;

        CutTree();
        DigRock();
        ScoopWater();
        UseWater();
        BuildBridge();
        LoadMaterial();
        UseRobot();
        RunFast(dt);

        showInfo = TutorialGame::GetGame()->ShowInfo();

        //Vector3 position = transform.GetPosition();
        //Vector3 p = FindGrid(Vector3(position.x, 2, position.z));
        //int index = p.x / 10 + (p.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
        //std::cout << "Index: " << index << std::endl;
        //std::cout << "Type: " << TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index).type << std::endl;
    //}
}

void PlayerObject::OnCollisionBegin(GameObject* otherObject) {

}

void PlayerObject::PlayerMovement(float dt) {
    Quaternion* qq = new Quaternion();
    //float yaw = Maths::RadiansToDegrees(atan2(-np.x, -np.z));
    //start->GetTransform().SetOrientation(qq->EulerAnglesToQuaternion(0, yaw, 0));
    float LeftX = TutorialGame::GetGame()->GetController()->GetNamedAxis("LeftX");
    float LeftY = TutorialGame::GetGame()->GetController()->GetNamedAxis("LeftY");
    //Debug::Print("LeftX: " + std::to_string(LeftX), Vector2(20, 20), Debug::BLUE);
    //Debug::Print("LeftY: " + std::to_string(LeftY), Vector2(20, 30), Debug::BLUE);

    Vector3 dir = Vector3(LeftX, 0, LeftY);
    physicsObject->SetRealDamping(0.962f);
    physicsObject->AddForce(dir * speed);
    if (LeftX != 0 && LeftY != 0) {
        renderObject->UpdateAnimation(dt);
        if (abs(LeftX) > abs(LeftY)) {
            if (LeftX > 0) {
                face = Vector3(1, 0, 0);
                transform.SetOrientation(qq->EulerAnglesToQuaternion(0, 90, 0));
            }
            else {
                face = Vector3(-1, 0, 0);
                transform.SetOrientation(qq->EulerAnglesToQuaternion(0, -90, 0));
            }
        }
        else {
            if (LeftY > 0) {
                face = Vector3(0, 0, 1);
                transform.SetOrientation(qq->EulerAnglesToQuaternion(0, 0, 0));
            }
            else {
                face = Vector3(0, 0, -1);
                transform.SetOrientation(qq->EulerAnglesToQuaternion(0, 180, 0));
            }
        }
    }
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
    if (TutorialGame::GetGame()->GetController()->GetNamedButton("Circle") && slot == 3) {
        int worldID1;
        doing = true;
        cutting = true;
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld().Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 10086 && closestCollision.rayDistance < 5.0f) {
                closest->SetFlag1(true);
                closest->GetTransform().SetScale(closest->GetTransform().GetScale() - Vector3(0.05, 0.05, 0.05));
                if (closest->GetTransform().GetScale().x < 0.1f) {
                    Vector3 position = FindGrid(closest->GetTransform().GetPosition());
                    int index = position.x / 10 + (position.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
                    GridNode& n = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index);
                    n.SetType(0);
                    closest->SetFlag1(false);
                    PlankObject* plank1 = TutorialGame::GetGame()->AddPlankToWorld(Vector3(closest->GetTransform().GetPosition().x, 5, closest->GetTransform().GetPosition().z));
                    TutorialGame::GetGame()->GetWorld().RemoveGameObject(closest, false);
                }
            }
        }
    }
}

void PlayerObject::DigRock() {  
    if (TutorialGame::GetGame()->GetController()->GetNamedButton("Circle") && slot == 2) {
        int worldID1;
        doing = true;
        digging = true;
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld().Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 10010 && closestCollision.rayDistance < 5.0f) {
                closest->SetFlag1(true);
                closest->GetTransform().SetScale(closest->GetTransform().GetScale() - Vector3(0.05, 0.05, 0.05));
                if (closest->GetTransform().GetScale().x < 0.1f) {
                    Vector3 position = FindGrid(closest->GetTransform().GetPosition());
                    int index = position.x / 10 + (position.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
                    GridNode& n = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index);
                    n.SetType(0);
                    closest->SetFlag1(false);
                    StoneObject* stone1 = TutorialGame::GetGame()->AddStoneToWorld(Vector3(closest->GetTransform().GetPosition().x, 5, closest->GetTransform().GetPosition().z));
                    TutorialGame::GetGame()->GetWorld().RemoveGameObject(closest, false);
                }
            }
        }
    }
}

void PlayerObject::ScoopWater() {
    if (TutorialGame::GetGame()->GetController()->GetNamedButton("Circle") && slot == 4) {
        doing = true;
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld().Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 10000 && closestCollision.rayDistance < 5.0f) {
                TutorialGame::GetGame()->GetBucket()->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
                TutorialGame::GetGame()->GetBucket()->SetWater(true);
            }
        }
    }
}

void PlayerObject::UseWater() {
    if (TutorialGame::GetGame()->GetController()->GetNamedButton("Circle") && slot == 4) {
        doing = true;        
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld().Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 23 && closestCollision.rayDistance < 5.0f && TutorialGame::GetGame()->GetBucket()->GetWater() == true) {
                WaterCarriage* waterCarriage = (WaterCarriage*)closest;
                waterCarriage->SetCarriageWater(100.0f);
                TutorialGame::GetGame()->GetTrain()->SetOnFire(false);
                TutorialGame::GetGame()->GetTrain()->SetFire(100.0f);
                TutorialGame::GetGame()->GetBucket()->SetWater(false);
                TutorialGame::GetGame()->GetBucket()->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
            }
        }
    }
}

void PlayerObject::BuildBridge() {
    if (TutorialGame::GetGame()->GetController()->GetNamedButton("Square") && slot == 5) {
        int worldID1;
        doing = true;
        Ray r = Ray(transform.GetPosition(), face);
        RayCollision closestCollision;
        if (TutorialGame::GetGame()->GetWorld().Raycast(r, closestCollision, true, this)) {
            GameObject* closest = (GameObject*)closestCollision.node;
            if (closest->GetTypeID() == 10000 && closestCollision.rayDistance < 5.0f) {
                bridgePosition = closest->GetTransform().GetPosition();
                TutorialGame::GetGame()->GetWorld().RemoveGameObject(closest, false);
                building = true;
            }
        }
    }
}

void PlayerObject::LoadMaterial() {
    if (TutorialGame::GetGame()->GetController()->GetNamedButton("Circle")) {
        if (slot == 5 || slot == 6) {
            Ray r = Ray(transform.GetPosition(), face);
            RayCollision closestCollision;
            if (TutorialGame::GetGame()->GetWorld().Raycast(r, closestCollision, true, this)) {
                GameObject* closest = (GameObject*)closestCollision.node;
                if (closest->GetTypeID() == 21 && closestCollision.rayDistance < 5.0f) {
                    carriage = (MaterialCarriage*)closest;
                }
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
    if (Maths::Vector::Length(TutorialGame::GetGame()->GetTrain()->GetLastPath() - position) < 14)
        isPath = true;

    return canConnect && isPath && notRail;
}

void PlayerObject::UseRobot() {
    if (TutorialGame::GetGame()->GetController()->GetNamedButton("Square")) {
        if (slot == 2 || slot == 3) {
            Ray r = Ray(transform.GetPosition(), face);
            RayCollision closestCollision;
            if (TutorialGame::GetGame()->GetWorld().Raycast(r, closestCollision, true, this)) {
                GameObject* closest = (GameObject*)closestCollision.node;
                if (closest->GetTypeID() == 11 && closestCollision.rayDistance < 5.0f) {
                    TutorialGame::GetGame()->GetRobot()->SetPlayer(this);
                    if (slot == 2) 
                        robotDig = true;
                    if (slot == 3) 
                        robotCut = true;
                }
            } 
        }
    }
}

void PlayerObject::RunFast(float dt) {
    if (coolDown < 2.0f) {
        coolDown += dt;
    }
    if (TutorialGame::GetGame()->GetController()->GetNamedButton("R2") && coolDown >= 2.0f) {
        running = true;
        coolDown = 0.0f;
        runPower = 0.11f;
    }
    if (running) {
        if (runPower <= 0) {
            running = false;
            speed = 50;
            physicsObject->SetRealDamping(0.962);
        }
        if (runPower > 0 && runPower < 0.05f) {
            speed = 50;
            physicsObject->SetRealDamping(0.825);
        }
        else if(runPower > 0 && runPower >= 0.05f){
            speed = 2000;
        }
        runPower -= dt;
    }
}