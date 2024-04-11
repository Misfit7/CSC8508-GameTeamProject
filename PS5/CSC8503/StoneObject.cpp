#include "StoneObject.h"
#include "TutorialGame.h"
#include "Window.h"

using namespace NCL::CSC8503;

void StoneObject::OnCollisionBegin(GameObject* otherObject) {
    if (putDown && otherObject->GetTypeID() == 1 && otherObject->GetSlot() == 0) {
        player = (PlayerObject*)otherObject;
        putDown = false;
        player->SetSlot(this->GetTypeID());
        player->SetSlotNum(player->GetSlotNum() + 1);
        SphereVolume* volume = new SphereVolume(1);
        SetBoundingVolume((CollisionVolume*)volume);
        transform.SetScale(Vector3(2, 2, 2));
        //TutorialGame::GetGame()->GetAudio()->PlayGet();
    }
    else if (putDown && otherObject->GetTypeID() == 1 && otherObject->GetSlot() == 6 && otherObject->GetSlotNum() < 3) {
        player = (PlayerObject*)otherObject;
        putDown = false;
        num = player->GetSlotNum() + 1;
        player->SetSlotNum(player->GetSlotNum() + 1);
        AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
        SetBoundingVolume((CollisionVolume*)volume);
        transform.SetScale(Vector3(2, 2, 2));
        //TutorialGame::GetGame()->GetAudio()->PlayGet();
    }
}

void StoneObject::Update(float dt) {
    if (putDown) {
        physicsObject->SetAngularVelocity(Vector3(0, 5, 0));
    }
    else {
        if (!inCarriage) {
            if (!loading) {
                Vector3 playerPosition = player->GetTransform().GetPosition();
                if (num == 1) transform.SetPosition(Vector3(playerPosition.x, playerPosition.y + 10, playerPosition.z));
                if (num == 2) transform.SetPosition(Vector3(playerPosition.x, playerPosition.y + 11, playerPosition.z));
                if (num == 3) transform.SetPosition(Vector3(playerPosition.x, playerPosition.y + 12, playerPosition.z));
                transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0));
                physicsObject->ClearForces();
                if (player->GetCarriage()) loading = true;
            }
            else {
                if (player->GetSlotNum() > 0 && player->GetSlot() == 6) {
                    carriage = player->GetCarriage();
                    if (carriage->GetStoneVector().size() < 10) {
                        carriage->AddStone(this);
                        loading = false;
                        inCarriage = true;
                        height = carriage->GetTransform().GetPosition().y + 4 + carriage->GetStoneVector().size();
                        SphereVolume* volume = new SphereVolume(1);
                        SetBoundingVolume((CollisionVolume*)volume);
                        transform.SetScale(Vector3(2, 2, 2));
                    }
                    player->SetSlotNum(player->GetSlotNum() - 1);
                    if (player->GetSlotNum() == 0) {
                        player->SetSlot(0);
                        player->SetCarriage(nullptr);
                        player = nullptr;
                    }
                }
            }
        }
        else {
            Vector3 carriagePosition = carriage->GetTransform().GetPosition();
            switch (carriage->GetDirection()) {
            case 1:
                transform.SetPosition(Vector3(carriagePosition.x - 1, height, carriagePosition.z));
                break;
            case 2:
                transform.SetPosition(Vector3(carriagePosition.x + 1, height, carriagePosition.z));
                break;
            case 3:
                transform.SetPosition(Vector3(carriagePosition.x, height, carriagePosition.z - 1));
                break;
            case 4:
                transform.SetPosition(Vector3(carriagePosition.x, height, carriagePosition.z + 1));
                break;
            default:
                break;
            }
        }

        if (!loading && !inCarriage) {
            if (TutorialGame::GetGame()->GetController()->GetNamedButton("Cross")) {
                if (!loading && !inCarriage) {
                    //TutorialGame::GetGame()->GetAudio()->PlayPut();
                    Vector3 position = transform.GetPosition();
                    Vector3 gridPosition = FindGrid(Vector3(position.x, 5, position.z) - player->GetFace() * 5.0f);
                    int index = gridPosition.x / 10 + (gridPosition.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
                    GridNode& n = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index);
                    if (n.type != 12345 && n.type != 10000 && n.type != 10086
                        && n.type != 10010 && n.type != 114514) {
                        putDown = true;
                        num = 1;
                        transform.SetPosition(Vector3(position.x, 5, position.z) - player->GetFace() * 5.0f);
                        AABBVolume* volume = new AABBVolume(Vector3(2, 2, 2));
                        SetBoundingVolume((CollisionVolume*)volume);
                        transform.SetScale(Vector3(4, 4, 4));
                        player->SetSlot(0);
                        player->SetSlotNum(0);
                        player = nullptr;
                    }
                }
            }
        }
    }
}