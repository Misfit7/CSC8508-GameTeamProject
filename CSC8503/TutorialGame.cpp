#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "AnimationObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"

#include "Assets.h"
#include <fstream>
using namespace NCL;
using namespace CSC8503;

TutorialGame* TutorialGame::instance = nullptr;

TutorialGame::TutorialGame() : controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse()) {
	std::cout << std::endl << "--------Initialising Game--------" << std::endl;

	world = new GameWorld();
	audio = new Audio(world);
#ifdef USEVULKAN
	renderer = new GameTechVulkanRenderer(*world);
	renderer->Init();
	renderer->InitStructures();
#else
	renderer = new GameTechRenderer(*world);
#endif
	physics = new PhysicsSystem(*world);

	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	world->SetGameState(GameState::LOADING);

	instance = this;

	playtime = 0.0f;
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes,
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
	delete audio;
}

void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		if (cameraMode == 1) {
			CameraUpdate();
		}
		else {
			world->GetMainCamera().UpdateCamera(dt);
		}
	}
	if (failure) {
		world->SetGameState(GameState::FAILURE);
		TutorialGame::GetGame()->GetAudio()->PlayFailure();
	}
	if (success) {
		if (level < 5) {
			renderer->GetUI()->SetSuccess(true);
			level++;
      playtime = 0.0f;
			float d;
			if (TutorialGame::GetGame()->IsNetworked())
				d = TutorialGame::GetGame()->GetTrain()->GetFloat2();
			else d = TutorialGame::GetGame()->GetTrain()->GetDistance();
			InitGameWorld(false, level);
			if (TutorialGame::GetGame()->IsNetworked())
				TutorialGame::GetGame()->GetTrain()->SetFloat2(d);
			else TutorialGame::GetGame()->GetTrain()->SetDistance(d);
		}
		else {
			world->SetGameState(GameState::FINISH);
			TutorialGame::GetGame()->GetAudio()->PlayWin();
		}
	}


	DrawPad();
	TutorialGame::UpdateKeys();

	playtime += dt;
	audio->Update();
	world->UpdateWorld(dt);
	float cv = carriage3->GetBoundingRadius();
	renderer->GetParticle()->Update(dt, carriage3, 2, Vector3(cv, cv, cv));
	renderer->Update(dt);
	renderer->GetUI()->Update(dt); //UI
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateLoading(float dt)
{
	audio->Update();
	renderer->Update(dt);
	renderer->GetUI()->Update(dt); //UI
	renderer->GetUI()->SetLoadingStep(assetsLoadedStep);
	renderer->Render();
	if (assetsLoadedStep == 5) world->SetGameState(CSC8503::GameState::MENU);
	AssetsLoading();
}

void TutorialGame::UpdatePaused(float dt)
{
	TutorialGame::UpdateKeys();
	audio->Update();
	renderer->Update(dt);
	renderer->GetUI()->Update(dt); //UI
	renderer->Render();
}

void TutorialGame::UpdateMenu(float dt)
{
	audio->Update();
	renderer->Update(dt);
	renderer->GetUI()->Update(dt); //UI
	renderer->Render();
}

void TutorialGame::UpdateFailure(float dt)
{
	audio->Update();
	renderer->Update(dt);
	renderer->GetUI()->Update(dt); //UI
	renderer->Render();
}

void TutorialGame::UpdateFinish(float dt)
{
	audio->Update();
	renderer->Update(dt);
	renderer->GetUI()->Update(dt); //UI
	renderer->Render();
}

void TutorialGame::DrawPad() {
	if (usePad) {
		if (player->GetSlot() == 0 || player->GetSlot() == 7) {
			Vector3 position = player->GetTransform().GetPosition();
			Vector3 p = player->FindGrid(Vector3(position.x, 4.5f, position.z));
			int index = p.x / 10 + (p.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
			pad->GetTransform().SetPosition(p);
			int type = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index).type;
			if (player->GetSlot() == 0) {
				if (type >= 10000)
					pad->GetRenderObject()->SetColour(Vector4(1, 0, 0, 0.4f));
				else
					pad->GetRenderObject()->SetColour(Vector4(1, 1, 0, 0.4f));
			}
			else if (player->GetSlot() == 7) {
				if (!player->CanPlaceRail())
					pad->GetRenderObject()->SetColour(Vector4(1, 0, 0, 0.4f));
				else
					pad->GetRenderObject()->SetColour(Vector4(1, 1, 0, 0.4f));
			}
		}
		else {
			Vector3 position = player->GetTransform().GetPosition();
			position = Vector3(position.x, 5, position.z) - player->GetFace() * 5.0f;
			Vector3 p = player->FindGrid(Vector3(position.x, 4.5f, position.z));
			int index = p.x / 10 + (p.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
			pad->GetTransform().SetPosition(position);
			int type = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index).type;
			if (type >= 10000)
				pad->GetRenderObject()->SetColour(Vector4(1, 0, 0, 0.4f));
			else
				pad->GetRenderObject()->SetColour(Vector4(1, 1, 0, 0.4f));
		}
	}
	else {
		pad->GetRenderObject()->SetColour(Vector4());
		pad->GetTransform().SetPosition(Vector3());
	}
}

void TutorialGame::UpdateKeys() {
	if (world->GetGameState() == GameState::PLAYING)
	{
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
			InitWorld(false, 2); //We can reset the simulation at any time with F1
			selectionObject = nullptr;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
			InitCamera(); //F2 will reset the camera to a specific default place
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::F3)) {
			renderer->ToggleNight();
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::G)) {
			useGravity = !useGravity; //Toggle gravity!
			physics->UseGravity(useGravity);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::M)) {
			success = true;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::P)) {
			usePad = !usePad;
		}
    
    if (Window::GetKeyboard()->KeyPressed(KeyCodes::B)) {
			showDebug = !showDebug;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::F9)) {
			cameraMode = 1;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::F10)) {
			cameraMode = 2;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::F7)) {
			world->ShuffleObjects(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyCodes::F8)) {
			world->ShuffleObjects(false);
		}

		if (lockedObject) {
			LockedObjectMovement();
		}

	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
		if (world->GetGameState() == GameState::PLAYING)
			world->SetGameState(GameState::PAUSED);
		else if (world->GetGameState() == GameState::PAUSED) {
			world->SetGameState(GameState::PLAYING);
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.

}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view = world->GetMainCamera().BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
	}
}

void TutorialGame::CameraUpdate() {
	if (train->IsSpawned()) {
		Vector3 objPos = train->GetTransform().GetPosition();
		Vector3 camPos = Vector3(objPos.x + 30, objPos.y + 130, objPos.z + 135);
		if (camPos.z > 270) camPos.z = 270;

		world->GetMainCamera().SetPosition(camPos);
		world->GetMainCamera().SetPitch(-50);
		world->GetMainCamera().SetYaw(0);
	}
}

void TutorialGame::InitGameWorld(bool networked, int level) {
	success = false;
	failure = false;
	winFlag = false;
	InitCamera();
	InitWorld(networked, level);
}

void TutorialGame::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);
	world->GetMainCamera().SetPitch(-45.0f);
	world->GetMainCamera().SetYaw(0.0f);
	world->GetMainCamera().SetPosition(Vector3(80, 100, 220));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld(bool networked, int level) {
	std::cout << std::endl << "--------Initialising Game Objects--------" << std::endl;
	world->ClearAndErase();
	physics->Clear();

	InitPositions(networked, level);
	InitGameExamples(networked, level);
	InitDefaultFloor();

}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(150, 2, 95));
}

void TutorialGame::InitPositions(bool networked, int level) {
    switch (level) {
    case 1:
        player1Position = Vector3(5, 4, 110);
        trainPosition = Vector3(30, 8.0f, 100);
        pickaxePosition = Vector3(25, 6.5f, 120);
        axePosition = Vector3(15, 8, 120);
        bucketPosition = Vector3(35, 6.5f, 120);
        robotPosition = Vector3(45, 4, 110);
        moosePosition = Vector3(180, 5, 120);
		dronePosition = Vector3(130, 5, 100);
		drone2Position = Vector3(0, 0, 0);
        if (networked) {
            player2Position = Vector3(15, 4, 110);
            player3Position = Vector3(25, 4, 110);
            player4Position = Vector3(35, 4, 110);
        }
        break;
    case 2:
        player1Position = Vector3(5, 4, 65);
        trainPosition = Vector3(30, 8.0f, 50);
        pickaxePosition = Vector3(20, 6.5f, 75);
        axePosition = Vector3(10, 8, 75);
        bucketPosition = Vector3(30, 6.5f, 75);
        robotPosition = Vector3(45, 4, 65);
        moosePosition = Vector3(150, 5, 100);
		dronePosition = Vector3(90, 5, 100);
		drone2Position = Vector3(0, 0, 0);
        if (networked) {
            player2Position = Vector3(15, 4, 65);
            player3Position = Vector3(25, 4, 65);
            player4Position = Vector3(35, 4, 65);
        }
        break;
    case 3:
        player1Position = Vector3(5, 4, 35);
        trainPosition = Vector3(30, 8.0f, 20);
        pickaxePosition = Vector3(20, 6.5f, 45);
        axePosition = Vector3(10, 8, 45);
        bucketPosition = Vector3(30, 6.5f, 45);
        robotPosition = Vector3(45, 4, 35);
        moosePosition = Vector3(150, 5, 35);
		dronePosition = Vector3(80, 5, 35);
		drone2Position = Vector3(100, 5, 55);
        if (networked) {
            player2Position = Vector3(15, 4, 35);
            player3Position = Vector3(25, 4, 35);
            player4Position = Vector3(35, 4, 35);
        }
        break;
     case 4:
            player1Position = Vector3(5, 4, 35);
            trainPosition = Vector3(30, 8.0f, 20);
            pickaxePosition = Vector3(20, 6.5f, 10);
            axePosition = Vector3(10, 8, 10);
            bucketPosition = Vector3(30, 6.5f, 10);
			robotPosition = Vector3(45, 4, 35);
			moosePosition = Vector3(60, 5, 50);
        dronePosition = Vector3(80, 5, 35);
		drone2Position = Vector3(100, 5, 55);
            if (networked) {
                player2Position = Vector3(15, 4, 35);
                player3Position = Vector3(25, 4, 35);
                player4Position = Vector3(35, 4, 35);
            }
            break;
		case 5:
			player1Position = Vector3(5, 4, 25);
			trainPosition = Vector3(30, 8.0f, 10);
			pickaxePosition = Vector3(20, 6.5f, 0);
			axePosition = Vector3(10, 8, 0);
			bucketPosition = Vector3(30, 6.5f, 0);
			robotPosition = Vector3(45, 4, 20);
			moosePosition = Vector3(60, 5, 20);
            dronePosition = Vector3(185, 5, 50);
		    drone2Position = Vector3(0, 0, 0);
			if (networked) {
				player2Position = Vector3(15, 4, 25);
				player3Position = Vector3(25, 4, 25);
				player4Position = Vector3(35, 4, 25);
			}
			break;
    default:
        break;
    }
}

void TutorialGame::InitGameExamples(bool networked, int level) {
    player = AddPlayerToWorld(player1Position, "Player1", 1, !networked);
    train = AddTrainToWorld(trainPosition, !networked);
    train->InitPaths(level);
    pickaxe = AddPickaxeToWorld(pickaxePosition, !networked);
    axe = AddAxeToWorld(axePosition, !networked);
    bucket = AddBucketToWorld(bucketPosition, !networked);
    if (networked) {
        player2 = AddPlayerToWorld(player2Position, "Player2", 2, false);
        player3 = AddPlayerToWorld(player3Position, "Player3", 3, false);
        player4 = AddPlayerToWorld(player4Position, "Player4", 4, false);
    }
    else {
        carriage1 = (MaterialCarriage*)(train->AddCarriage(21, !networked));
        carriage2 = (ProduceCarriage*)(train->AddCarriage(22, !networked));
        carriage3 = (WaterCarriage*)(train->AddCarriage(23, !networked));
        carriage1->SetProduceCarriage(carriage2);
        carriage2->SetMaterialCarriage(carriage1);
    }
    pad = AddPadToWorld();
    AddSceneToWorld(level);

	robot = AddRobotToWorld(robotPosition);
    moose = AddMooseToWorld(moosePosition);
	if (!networked) {
		drone = AddDroneToWorld(dronePosition);
		if (drone2Position != Vector3(0, 0, 0)) drone2 = AddDroneToWorld(drone2Position);
	}
}

bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Left)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
		//Vector3 position = findNearestGridCenter(Vector3(3, 12.5, 0));
		//Debug::Print("x:" + std::to_string(position.x), Vector2(5, 85));
		//Debug::Print("y:" + std::to_string(position.y), Vector2(5, 100));
	}
	return false;
}
void TutorialGame::MoveSelectedObject() {
	// renderer -> DrawString ( " Click Force : " + std :: to_string ( forceMagnitude ) , Vector2 (10 , 20));
	Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Right)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}

	if (Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::W)) {
		selectionObject->GetPhysicsObject()->AddForceAtPosition(Vector3(100, 0, 0), selectionObject->GetTransform().GetPosition());
	}
	else if (Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::A)) {
		selectionObject->GetPhysicsObject()->AddForceAtPosition(Vector3(0, 0, 100), selectionObject->GetTransform().GetPosition());
	}
	else if (Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::S)) {
		selectionObject->GetPhysicsObject()->AddForceAtPosition(Vector3(-100, 0, 0), selectionObject->GetTransform().GetPosition());
	}
	else if (Window::GetKeyboard()->KeyHeld(NCL::KeyCodes::D)) {
		selectionObject->GetPhysicsObject()->AddForceAtPosition(Vector3(0, 0, -100), selectionObject->GetTransform().GetPosition());
	}
	else {
		selectionObject->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
	}

}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);
	float invCubeMass = 5;  // how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 30;  // constraint distance
	float cubeDistance = 20;  // distance between links

	Vector3 startPos = Vector3(5, 100, 5);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		world->AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::HoldObject()
{
	object->GetTransform().SetPosition(object->PlayerFront());
}