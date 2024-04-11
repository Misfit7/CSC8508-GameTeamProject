#include "CollectableObject.h"
#include "TutorialGame.h"
#include "Window.h"

using namespace NCL::CSC8503;

void CollectableObject::Update(float dt) {
    if (putDown) {
        physicsObject->SetAngularVelocity(Vector3(0, 5, 0));
    }
    else {
        if (player) {
            Vector3 playerPosition = player->GetTransform().GetPosition();
            transform.SetPosition(Vector3(playerPosition.x, playerPosition.y + 8, playerPosition.z));
            transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0));

            bool RPressed = false;
            if (player->GetNetworkObject()->GetNetworkID() == 1 && Window::GetKeyboard()->KeyPressed(KeyCodes::R))
                RPressed = true;
            else
                RPressed = player->GetButton(4);

            if (RPressed) {
                TutorialGame::GetGame()->GetAudio()->PlayPut();
                Vector3 position = transform.GetPosition();
                Vector3 gridPosition = FindGrid(Vector3(position.x, 5, position.z) - player->GetFace() * 5.0f);
                int index = gridPosition.x / 10 + (gridPosition.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
                GridNode& n = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index);
                if (n.type != 12345 && n.type != 10000 && n.type != 10086
                    && n.type != 10010 && n.type != 114514) {
                    putDown = true;
                    num = 1;
                    if (typeID != 3)transform.SetPosition(Vector3(position.x, 6.5f, position.z) - player->GetFace() * 5.0f);
                    else
                        transform.SetPosition(Vector3(position.x, 8, position.z) - player->GetFace() * 5.0f);
                    player->SetSlot(0);
                    player->SetSlotNum(0);
                    player = nullptr;
                }
            }

            if (typeID == 2 && TutorialGame::GetGame()->GetRobot()->IsDigging()) {
                player->SetSlot(0);
                player->SetSlotNum(0);
                player = nullptr;
                robot = TutorialGame::GetGame()->GetRobot();
            }
            else if (typeID == 3 && TutorialGame::GetGame()->GetRobot()->IsCutting()) {
                player->SetSlot(0);
                player->SetSlotNum(0);
                player = nullptr;
                robot = TutorialGame::GetGame()->GetRobot();
            }
        }
        else if (robot) {
            Vector3 robotPosition = robot->GetTransform().GetPosition();
            transform.SetPosition(Vector3(robotPosition.x, robotPosition.y + 8, robotPosition.z));
            transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0));
            if (!robot->IsCutting() && !robot->IsDigging()) {
                if (typeID == 3)transform.SetPosition(Vector3(transform.GetPosition().x, 8, transform.GetPosition().z));
                else transform.SetPosition(Vector3(transform.GetPosition().x, 6.5f, transform.GetPosition().z));
                putDown = true;
                robot = nullptr;
            }
        }
    }
}

void CollectableObject::OnCollisionBegin(GameObject* otherObject) {
    if (putDown && otherObject->GetTypeID() == 1 && otherObject->GetSlot() == 0) {
        player = (PlayerObject*)otherObject;
        putDown = false;
        player->SetSlot(this->GetTypeID());
        TutorialGame::GetGame()->GetAudio()->PlayGet();
    }
}

float CollectableObject::Distance(const Vector3& v1, const Vector3& v2) {
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    float dz = v1.z - v2.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

Vector3 CollectableObject::PlayerFront()
{
    Vector3 playerPos = player->GetTransform().GetPosition();
    Quaternion facingDir = player->GetTransform().GetOrientation();
    Vector3 ObjectOffset(0, 2, -5);
    Vector3 ObjectPos = facingDir * ObjectOffset;
    ObjectOffset = facingDir * ObjectOffset;
    Vector3 finalObjectPos = ObjectPos + playerPos;
    return finalObjectPos;
}