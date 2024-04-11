#include "PlankObject.h"
#include "TutorialGame.h"
#include "NetworkedGame.h"
#include "Window.h"

using namespace NCL::CSC8503;

void PlankObject::OnCollisionBegin(GameObject* otherObject) {
	if (!TutorialGame::GetGame()->IsNetworked() || NetworkedGame::GetNetworkedGame()->IsServer()) {
		if (!placed) {
			if (putDown && otherObject->GetTypeID() == 1 && otherObject->GetSlot() == 0) {
				player = (PlayerObject*)otherObject;
				putDown = false;
				player->SetSlot(this->GetTypeID());
				player->SetSlotNum(player->GetSlotNum() + 1);
				AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
				SetBoundingVolume((CollisionVolume*)volume);
				transform.SetScale(Vector3(2, 2, 2));
				TutorialGame::GetGame()->GetAudio()->PlayGet();
			}
			else if (putDown && otherObject->GetTypeID() == 1 && otherObject->GetSlot() == 5 && otherObject->GetSlotNum() < 3) {
				player = (PlayerObject*)otherObject;
				putDown = false;
				num = player->GetSlotNum() + 1;
				player->SetSlotNum(player->GetSlotNum() + 1);
				AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
				SetBoundingVolume((CollisionVolume*)volume);
				transform.SetScale(Vector3(2, 2, 2));
				TutorialGame::GetGame()->GetAudio()->PlayGet();
			}
		}
	}
}

void PlankObject::Update(float dt) {
	flag2 = putDown;
	if (!placed && !putDown && !inCarriage) {
		CreateBridge();
	}
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
	if (putDown && !placed) {
		physicsObject->SetAngularVelocity(Vector3(0, 5, 0));
	}
	else if (!putDown) {
		if (!inCarriage && !placed) {
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
				if (player->GetSlotNum() > 0 && player->GetSlot() == 5) {
					carriage = player->GetCarriage();
					if (carriage->GetPlankVector().size() < 10) {
						carriage->AddPlank(this);
						loading = false;
						inCarriage = true;
						height = carriage->GetTransform().GetPosition().y + 4 + carriage->GetPlankVector().size();
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
		else if (inCarriage) {
			Vector3 carriagePosition = carriage->GetTransform().GetPosition();
			switch (carriage->GetDirection()) {
			case 1:
				transform.SetPosition(Vector3(carriagePosition.x + 1, height, carriagePosition.z));
				break;
			case 2:
				transform.SetPosition(Vector3(carriagePosition.x - 1, height, carriagePosition.z));
				break;
			case 3:
				transform.SetPosition(Vector3(carriagePosition.x, height, carriagePosition.z + 1));
				break;
			case 4:
				transform.SetPosition(Vector3(carriagePosition.x, height, carriagePosition.z - 1));
				break;
			default:
				break;
			}
		}

		if (!loading && !inCarriage && !placed) {
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
	if (placed) {
		flag1 = true;
	}
}

void PlankObject::CreateBridge() {
	if (player->IsBuilding() && !putDown && !inCarriage && num == 1) {
		placed = true;
		putDown = true;
		//Vector3 position = FindGrid(Vector3(transform.GetPosition().x, 4.5f, transform.GetPosition().z));
		Vector3 position = player->GetBridgePosition();
		int index = position.x / 10 + (position.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
		GridNode& n = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index);
		transform.SetScale(Vector3(10, 10, 6));
		physicsObject->SetAngularVelocity(Vector3());
		n.SetType(0);
		transform.SetPosition(Vector3(position.x, 3.5f, position.z));
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
		player->SetBuilding(false);
		player = nullptr;
	}
}