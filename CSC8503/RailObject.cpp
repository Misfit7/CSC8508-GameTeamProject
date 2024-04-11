#include "RailObject.h"
#include "TutorialGame.h"
#include "NetworkedGame.h"
#include "NavigationGrid.h"
#include "Window.h"

using namespace NCL::CSC8503;

void RailObject::OnCollisionBegin(GameObject* otherObject) {
    if (!TutorialGame::GetGame()->IsNetworked() || NetworkedGame::GetNetworkedGame()->IsServer()) {
        if (!placed) {
            if (!inCarriage) {
                if (putDown && otherObject->GetTypeID() == 1 && otherObject->GetSlot() == 0) {
                    player = (PlayerObject*)otherObject;
                    putDown = false;
                    player->SetSlot(this->GetTypeID());
                    player->SetSlotNum(player->GetSlotNum() + 1);
                    TutorialGame::GetGame()->GetAudio()->PlayGet();
                }
                else if (putDown && otherObject->GetTypeID() == 1 && otherObject->GetSlot() == 7 && otherObject->GetSlotNum() < 3) {
                    player = (PlayerObject*)otherObject;
                    putDown = false;
                    num = player->GetSlotNum() + 1;
                    player->SetSlotNum(player->GetSlotNum() + 1);
                    TutorialGame::GetGame()->GetAudio()->PlayGet();
                }
            }
        }
    }
}

void RailObject::Update(float dt) {
    if (inCarriage) {
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
    else if (!putDown) {
        Vector3 playerPosition = player->GetTransform().GetPosition();
        if (num == 1) transform.SetPosition(Vector3(playerPosition.x, playerPosition.y + 10, playerPosition.z));
        if (num == 2) transform.SetPosition(Vector3(playerPosition.x, playerPosition.y + 11, playerPosition.z));
        if (num == 3) transform.SetPosition(Vector3(playerPosition.x, playerPosition.y + 12, playerPosition.z));
        transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0));
        physicsObject->ClearForces();
        //std::cout << "Num: " << num << std::endl;

        if (!inCarriage) {
            bool RPressed = false;
            if (player->GetNetworkObject()->GetNetworkID() == 1)
                RPressed = Window::GetKeyboard()->KeyPressed(KeyCodes::R);
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
                    transform.SetPosition(Vector3(position.x, 5, position.z) - player->GetFace() * 5.0f);
                    player->SetSlot(0);
                    player->SetSlotNum(0);
                    player = nullptr;
                }
            }
        }
        if (!putDown && !placed && !inCarriage)
            PlaceRail();
        if (player) {
            if (player->IsPlacing1()) {
                if (num == 2) {
                    num = 1;
                    player->SetPlacing1(false);
                }
            }
            if (player->IsPlacing2()) {
                if (num == 3) {
                    num = 2;
                    player->SetPlacing2(false);
                }
            }
            //std::cout << "RailNum: " << num << std::endl;
        }
    }
    else if (putDown && !placed) {
        physicsObject->SetAngularVelocity(Vector3(0, 5, 0));
    }
}

//1 ×ó,2 ÓÒ,3 Ç°,4 ºó

//int GetDirection(Vector3 point1, Vector3 point2)
//{
//    float deltaX = point2.x - point1.x;
//    float deltaY = point2.y - point1.y;
//
//    if (std::abs(deltaX) > std::abs(deltaY)) {
//        if (deltaX > 0)
//            return 2;
//        else
//            return 1;
//    }
//    else {
//        if (deltaY > 0)
//            return 3;
//        else
//            return 4;
//    }
//}

//int RailObject::GetRailDirection(const Vector3& position)
//{
//
//    static std::vector<Vector3> values;
//
//    values.push_back(position);
//
//    if (values.size() == 2)
//    {
//        return GetDirection(values[0], values[1]);
//    }
//    else if (values.size() == 3)
//    {
//        values.erase(values.begin());
//        return GetDirection(values[0], values[1]);
//    }
//}

void RailObject::PlaceRail() {
    bool FPressed = false;
    TrainObject* train = TutorialGame::GetGame()->GetTrain();

    if (player->GetNetworkObject()->GetNetworkID() == 1)
        FPressed = Window::GetKeyboard()->KeyPressed(KeyCodes::F);
    else
        FPressed = player->GetButton(7);

    if (FPressed && player->GetSlot() == 7 && num == 1) {
        if (!putDown && !inCarriage) {
            TutorialGame::GetGame()->GetAudio()->PlayGet();
            bool canConnect = false;
            bool isPath = false;
            int connectedIndex = -1;
            RailObject* lastRail = nullptr;
            Vector3 position = FindGrid(Vector3(transform.GetPosition().x, 4.5f, transform.GetPosition().z));
            int index = position.x / 10 + (position.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
            GridNode& n = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index);
            for (int i = 0; i < 4; ++i) {
                if (n.connected[i]) {
                    if (n.connected[i]->type == 7) {
                        canConnect = true;
                        connectedIndex = i;
                        break;
                    }
                }
            }
            if ((train->GetLastPath() - Vector3(0, 3.5f, 0) - position).Length() < 14)
                isPath = true;
            if (n.type == 0 && canConnect && isPath) {
                placed = true;
                putDown = true;
                transform.SetScale(Vector3(10, 10, 10));
                n.SetType(7);
                n.SetRail(this);
                transform.SetPosition(position);
                physicsObject->SetAngularVelocity(Vector3());
                train->AddPath(position);
                train->AddCarriagePath(position);
                switch (connectedIndex) {
                case 0:
                    transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 90, 0));
                    direction = 1;
                    lastRail = n.connected[connectedIndex]->GetRail();
                    if (lastRail->GetDirection() == 2) {
                        lastRail->GetRenderObject()->SetMesh(railTurnMesh);
                        lastRail->GetRenderObject()->SetDefaultTexture(railTurnTex);
                        lastRail->GetRenderObject()->SetBumpTexture(railTurnBumpTex);
                        if (TutorialGame::GetGame()->IsNetworked()) {
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailFlag(true);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailTag(7);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailNetworkID(lastRail->GetNetworkObject()->GetNetworkID());
                        }
                        lastRail->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 90, 0));
                    }
                    else if (lastRail->GetDirection() == 3) {
                        lastRail->GetRenderObject()->SetMesh(railTurnMesh);
                        lastRail->GetRenderObject()->SetDefaultTexture(railTurnTex);
                        lastRail->GetRenderObject()->SetBumpTexture(railTurnBumpTex);
                        if (TutorialGame::GetGame()->IsNetworked()) {
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailFlag(true);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailTag(7);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailNetworkID(lastRail->GetNetworkObject()->GetNetworkID());
                        }
                        lastRail->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0))
                            .SetPosition(Vector3(lastRail->GetTransform().GetPosition().x - 10, lastRail->GetTransform().GetPosition().y + 1, lastRail->GetTransform().GetPosition().z + 8))
                            .SetScale(Vector3(17, 1, 17));
                    }
                    break;
                case 1:
                    transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 90, 0));
                    direction = 0;
                    lastRail = n.connected[connectedIndex]->GetRail();
                    if (lastRail->GetDirection() == 2) {
                        lastRail->GetRenderObject()->SetMesh(railTurnMesh);
                        lastRail->GetRenderObject()->SetDefaultTexture(railTurnTex);
                        lastRail->GetRenderObject()->SetBumpTexture(railTurnBumpTex);
                        if (TutorialGame::GetGame()->IsNetworked()) {
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailFlag(true);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailTag(7);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailNetworkID(lastRail->GetNetworkObject()->GetNetworkID());
                        }
                        lastRail->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 180, 0));
                    }
                    else if (lastRail->GetDirection() == 3) {
                        lastRail->GetRenderObject()->SetMesh(railTurnMesh);
                        lastRail->GetRenderObject()->SetDefaultTexture(railTurnTex);
                        lastRail->GetRenderObject()->SetBumpTexture(railTurnBumpTex);
                        if (TutorialGame::GetGame()->IsNetworked()) {
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailFlag(true);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailTag(7);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailNetworkID(lastRail->GetNetworkObject()->GetNetworkID());
                        }
                        lastRail->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 270, 0))
                            .SetPosition(Vector3(lastRail->GetTransform().GetPosition().x - 8, lastRail->GetTransform().GetPosition().y + 1, lastRail->GetTransform().GetPosition().z - 8))
                            .SetScale(Vector3(17, 1, 17));
                    }
                    break;
                case 2:
                    transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0));
                    direction = 3;
                    lastRail = n.connected[connectedIndex]->GetRail();
                    if (lastRail->GetDirection() == 0) {
                        lastRail->GetRenderObject()->SetMesh(railTurnMesh);
                        lastRail->GetRenderObject()->SetDefaultTexture(railTurnTex);
                        lastRail->GetRenderObject()->SetBumpTexture(railTurnBumpTex);
                        if (TutorialGame::GetGame()->IsNetworked()) {
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailFlag(true);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailTag(7);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailNetworkID(lastRail->GetNetworkObject()->GetNetworkID());
                        }
                        lastRail->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 90, 0))
                            .SetPosition(Vector3(lastRail->GetTransform().GetPosition().x + 6, lastRail->GetTransform().GetPosition().y + 1, lastRail->GetTransform().GetPosition().z + 10))
                            .SetScale(Vector3(15, 10, 15));
                    }
                    else if (lastRail->GetDirection() == 1) {
                        lastRail->GetRenderObject()->SetMesh(railTurnMesh);
                        lastRail->GetRenderObject()->SetDefaultTexture(railTurnTex);
                        lastRail->GetRenderObject()->SetBumpTexture(railTurnBumpTex);
                        if (TutorialGame::GetGame()->IsNetworked()) {
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailFlag(true);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailTag(7);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailNetworkID(lastRail->GetNetworkObject()->GetNetworkID());
                        }
                        lastRail->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 180, 0))
                            .SetPosition(Vector3(lastRail->GetTransform().GetPosition().x + 10, lastRail->GetTransform().GetPosition().y + 1, lastRail->GetTransform().GetPosition().z - 7))
                            .SetScale(Vector3(15, 10, 15));
                    }
                    break;
                case 3:
                    transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0));
                    direction = 2;
                    lastRail = n.connected[connectedIndex]->GetRail();
                    if (lastRail->GetDirection() == 0) {
                        lastRail->GetRenderObject()->SetMesh(railTurnMesh);
                        lastRail->GetRenderObject()->SetDefaultTexture(railTurnTex);
                        lastRail->GetRenderObject()->SetBumpTexture(railTurnBumpTex);
                        if (TutorialGame::GetGame()->IsNetworked()) {
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailFlag(true);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailTag(7);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailNetworkID(lastRail->GetNetworkObject()->GetNetworkID());
                        }
                        lastRail->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0));
                    }
                    else if (lastRail->GetDirection() == 1) {
                        lastRail->GetRenderObject()->SetMesh(railTurnMesh);
                        lastRail->GetRenderObject()->SetDefaultTexture(railTurnTex);
                        lastRail->GetRenderObject()->SetBumpTexture(railTurnBumpTex);
                        if (TutorialGame::GetGame()->IsNetworked()) {
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailFlag(true);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailTag(7);
                            NetworkedGame::GetNetworkedGame()->SetUpdateRailNetworkID(lastRail->GetNetworkObject()->GetNetworkID());
                        }
                        lastRail->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 270, 0));
                    }
                    break;
                default:
                    break;
                }
                player->SetSlotNum(player->GetSlotNum() - 1);
                if (player->GetSlotNum() == 0) {
                    player->SetSlot(0);
                }
                else if (player->GetSlotNum() == 1) {
                    player->SetPlacing1(true);
                }
                else if (player->GetSlotNum() == 2) {
                    player->SetPlacing1(true);
                    player->SetPlacing2(true);
                }
                player = nullptr;
            }
        }
    }
}

void RailObject::UploadAssets(Mesh* m, Texture* t, Texture* bt) {
    railTurnMesh = m;
    railTurnTex = t;
    railTurnBumpTex = bt;
}
