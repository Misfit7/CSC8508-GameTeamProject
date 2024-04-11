#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"

#include "../PS5Core/PS5Window.h"


using namespace NCL;
using namespace CSC8503;

TutorialGame* TutorialGame::instance = nullptr;

TutorialGame::TutorialGame(GameWorld& inWorld, GameTechRendererInterface& inRenderer, PhysicsSystem& inPhysics)
	: world(inWorld),
	renderer(inRenderer),
	physics(inPhysics)
{
	level = 1;
	forceMagnitude = 10.0f;
	useGravity = false;
	inSelectionMode = false;

	NCL::PS5::PS5Window* w = (NCL::PS5::PS5Window*)Window::GetWindow();

	world.GetMainCamera().SetController(*w->GetController());

	controller = w->GetController();

	InitialiseAssets();

	world.SetGameState(GameState::START);

	instance = this;
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes,
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	InitMeshes();
	InitTextures();
	InitShaders();
	InitAnimations();
	InitGameWorld();
}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;
}

void TutorialGame::UpdateGame(float dt) {
	switch (world.GetGameState())
	{
	case START:
		UpdateStart(dt);
		break;
	case PLAYING:
		UpdatePlaying(dt);
		break;
	case PAUSED:
		UpdatePaused(dt);
		break;
	case FAILURE:
		UpdateFailure(dt);
		break;
	case FINISH:
		UpdateFinished(dt);
		break;
	case EXIT:
		isExit = true;
		break;
	default:
		break;
	}
}

void TutorialGame::UpdateStart(float dt) {
	Debug::Print("Train Bob", Vector2(45, 20), Debug::RED);
	Debug::Print("Press X to start", Vector2(40, 30), Debug::BLUE);
	if (TutorialGame::GetGame()->GetController()->GetNamedButton("Cross"))
		world.SetGameState(GameState::PLAYING);
}

void TutorialGame::UpdatePlaying(float dt) {
	if (!lockedCamera)
		world.GetMainCamera().UpdateCamera(dt);
	else
		CameraUpdate();

	UpdateKeys();
	DrawPad();

	if (failure) {
		world.SetGameState(GameState::FAILURE);
	}
	else if (success) {
		if (level < 5) {
			infoString = "You Win!!!";
			infoCounter = 3.0f;
			infoPos = Vector2(40, 80);
			infoColour = Debug::CYAN;
			level++;
			playTime = 0.0f;
			InitGameWorld(level);
		}
		else {
			world.SetGameState(GameState::FINISH);
		}
	}
	else {
		if (showInfo) {
			Debug::Print("Framerate: " + std::to_string((int)(1 / dt)) + " fps", Vector2(0, 5), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Delta time: " + std::to_string(dt * 1000) + "ms", Vector2(0, 10), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Level: " + std::to_string(level), Vector2(0, 15), Vector4(1, 1, 1, 0.5f));
			ConvertTime(playTime, hours, minutes, seconds);
			Debug::Print("Time: " + std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds), Vector2(0, 20), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Distance: " + std::to_string((int)train->GetDistance()), Vector2(0, 25), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Speed: " + std::to_string(train->GetForce()), Vector2(0, 30), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Position: " + std::to_string((int)player->GetTransform().GetPosition().x) + "," + std::to_string((int)player->GetTransform().GetPosition().y) + "," + std::to_string((int)player->GetTransform().GetPosition().z), Vector2(0, 35), Vector4(1, 1, 1, 0.5f));
			UpdateSlotString();
			Debug::Print("Inventory: " + slotString, Vector2(0, 40), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Item Quantity: " + std::to_string(player->GetSlotNum()), Vector2(0, 45), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Player Speed: " + std::to_string(player->GetSpeed()), Vector2(0, 50), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Press R1 to hide information", Vector2(0, 65), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Press Options to pause", Vector2(0, 70), Vector4(1, 1, 1, 0.5f));
		}
		else {
			Debug::Print("Level: " + std::to_string(level), Vector2(0, 5), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Press R1 to show more information", Vector2(0, 10), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Press Options to pause", Vector2(0, 15), Vector4(1, 1, 1, 0.5f));
			std::string robot = "";
			switch (TutorialGame::GetGame()->GetRobot()->GetStateID()) {
			case 0:
				if (TutorialGame::GetGame()->GetPlayer()->GetSlot() == 2 || TutorialGame::GetGame()->GetPlayer()->GetSlot() == 3)
					robot = "Robot: Come to me and Press Square, I'll help u~";
				else
					robot = "Robot: Available";
				break;
			case 1:
				robot = "Robot: I'm following u! Press L1 to make me work!";
				break;
			case 2:
				robot = "Robot: I'm finding the nearest tree! Press L1 to call me back!";
				break;
			case 3:
				robot = "Robot: I'm finding the nearest rock! Press L1 to call me back!";
				break;
			case 4:
				robot = "Robot: I'm cutting tree... Press L1 to call me back!";
				break;
			case 5:
				robot = "Robot: I'm digging rock... Press L1 to call me back!";
				break;
			case 6:
				robot = "Robot: I'm coming back to u!";
				break;
			default:
				break;
			}
			Debug::Print(robot, Vector2(0, 95), Debug::BLUE);
		}
	}

	if (infoCounter > 0) {
		infoCounter -= dt;
		if (infoCounter <= 0) {
			infoString = "";
			infoCounter = 0.0f;
		}
	}

	Debug::Print(infoString, infoPos, infoColour);

	playTime += dt;
	world.UpdateWorld(dt);
	physics.Update(dt);
}

void TutorialGame::UpdatePaused(float dt) {
	Debug::Print("Press X to continue", Vector2(30, 40), Debug::BLUE);
	Debug::Print("Press O to exit", Vector2(35, 60), Debug::RED);
	if (TutorialGame::GetGame()->GetController()->GetNamedButton("Cross")) {
		world.SetGameState(GameState::PLAYING);
	}
	else if (TutorialGame::GetGame()->GetController()->GetNamedButton("Circle")) {
		world.SetGameState(GameState::EXIT);
	}
}

void TutorialGame::UpdateFailure(float dt) {
	Debug::Print("You Lose!", Vector2(37, 40), Debug::RED);
	Debug::Print("Press X to restart", Vector2(30, 50), Debug::BLUE);
	Debug::Print("Press O to exit", Vector2(35, 60), Debug::RED);
	if (TutorialGame::GetGame()->GetController()->GetNamedButton("Cross")) {
		InitGameWorld(level);
		world.SetGameState(GameState::PLAYING);
	}
	else if (TutorialGame::GetGame()->GetController()->GetNamedButton("Circle")) {
		world.SetGameState(GameState::EXIT);
	}
}

void TutorialGame::UpdateFinished(float dt) {
	Debug::Print("You Win!!!", Vector2(37, 40), Debug::CYAN);
	Debug::Print("Press X to restart", Vector2(30, 50), Debug::BLUE);
	Debug::Print("Press O to exit", Vector2(35, 60), Debug::RED);
	if (TutorialGame::GetGame()->GetController()->GetNamedButton("Cross")) {
		InitGameWorld(1);
		world.SetGameState(GameState::PLAYING);
	}
	else if (TutorialGame::GetGame()->GetController()->GetNamedButton("Circle")) {
		world.SetGameState(GameState::EXIT);
	}
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (TutorialGame::GetGame()->GetController()->ButtonPressed("L1")) {
		lockedCamera = !lockedCamera;
	}

	if (TutorialGame::GetGame()->GetController()->GetNamedButton("Options")) {
		world.SetGameState(GameState::PAUSED);
	}

	if (TutorialGame::GetGame()->GetController()->ButtonPressed("R1")) {
		showInfo = !showInfo;
	}

	if (TutorialGame::GetGame()->GetController()->ButtonPressed("L2")) {
		success = true;
	}

	if (TutorialGame::GetGame()->GetController()->ButtonPressed("Triangle")) {
		usePad = !usePad;
	}
}

void TutorialGame::CameraUpdate() {
	if (train) {
		Vector3 objPos = train->GetTransform().GetPosition();
		Vector3 camPos = Vector3(objPos.x + 30, objPos.y + 130, objPos.z + 135);
		if (camPos.z > 270) camPos.z = 270;

		world.GetMainCamera().SetPosition(camPos);
		world.GetMainCamera().SetPitch(-50);
		world.GetMainCamera().SetYaw(0);
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view = world.GetMainCamera().BuildViewMatrix();
	Matrix4 camWorld = Matrix::Inverse(view);

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis = Vector::Normalise(fwdAxis);

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

void TutorialGame::DebugObjectMovement() {
	//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyCodes::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitGameWorld(int level) {
	success = false;
	failure = false;
	InitCamera();
	InitWorld(level);
}

void TutorialGame::InitCamera() {
	world.GetMainCamera().SetNearPlane(0.1f);
	world.GetMainCamera().SetFarPlane(500.0f);
	world.GetMainCamera().SetPitch(-45.0f);
	world.GetMainCamera().SetYaw(0.0f);
	world.GetMainCamera().SetPosition(Vector3(80, 100, 220));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld(int level) {
	std::cout << std::endl << "--------Initialising Game Objects--------" << std::endl;
	world.ClearAndErase();
	physics.Clear();

	InitPositions(level);
	InitDefaultFloor();
	InitGameExamples(level);
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(150, 2, 95));
}

void TutorialGame::InitPositions(int level) {
	switch (level) {
	case 1:
		playerPosition = Vector3(5, 4, 110);
		trainPosition = Vector3(30, -2.5f, 100);
		pickaxePosition = Vector3(25, 6.5f, 120);
		axePosition = Vector3(15, 8, 120);
		bucketPosition = Vector3(35, 6.5f, 120);
		robotPosition = Vector3(45, 7, 110);
		moosePosition = Vector3(140, 5, 100);
		break;
	case 2:
		playerPosition = Vector3(5, 4, 65);
		trainPosition = Vector3(30, -2.5f, 50);
		pickaxePosition = Vector3(20, 6.5f, 75);
		axePosition = Vector3(10, 8, 75);
		bucketPosition = Vector3(30, 6.5f, 75);
		robotPosition = Vector3(45, 7, 65);
		moosePosition = Vector3(140, 5, 100);
		break;
	case 3:
		playerPosition = Vector3(5, 4, 35);
		trainPosition = Vector3(30, -2.5f, 20);
		pickaxePosition = Vector3(20, 6.5f, 45);
		axePosition = Vector3(10, 8, 45);
		bucketPosition = Vector3(30, 6.5f, 45);
		robotPosition = Vector3(45, 7, 35);
		moosePosition = Vector3(40, 5, 50);
		break;
	case 4:
		playerPosition = Vector3(5, 4, 35);
		trainPosition = Vector3(30, -2.5f, 20);
		pickaxePosition = Vector3(20, 6.5f, 10);
		axePosition = Vector3(10, 8, 10);
		bucketPosition = Vector3(30, 6.5f, 10);
		robotPosition = Vector3(45, 4, 35);
		moosePosition = Vector3(60, 5, 50);
		break;
	case 5:
		playerPosition = Vector3(5, 4, 25);
		trainPosition = Vector3(30, -2.5f, 10);
		pickaxePosition = Vector3(20, 6.5f, 0);
		axePosition = Vector3(10, 8, 0);
		bucketPosition = Vector3(30, 6.5f, 0);
		robotPosition = Vector3(45, 4, 20);
		moosePosition = Vector3(60, 5, 20);
		break;
	default:
		break;
	}
}

void TutorialGame::InitGameExamples(int level) {
	axe = AddAxeToWorld(axePosition);
	pickaxe = AddPickaxeToWorld(pickaxePosition);
	bucket = AddBucketToWorld(bucketPosition);
	player = AddPlayerToWorld(playerPosition);
	train = AddTrainToWorld(trainPosition);
	train->InitPaths(level);
	carriage1 = (MaterialCarriage*)(train->AddCarriage(21));
	carriage2 = (ProduceCarriage*)(train->AddCarriage(22));
	carriage3 = (WaterCarriage*)(train->AddCarriage(23));
	carriage1->SetProduceCarriage(carriage2);
	carriage2->SetMaterialCarriage(carriage1);
	pad = AddPadToWorld();
	AddSceneToWorld(level);
	moose = AddMooseToWorld(moosePosition);
	robot = AddRobotToWorld(robotPosition);
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around.

*/
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
		//Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Left)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(world.GetMainCamera());

			RayCollision closestCollision;
			if (world.Raycast(ray, closestCollision, true)) {
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
		//Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	//Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Right)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(world.GetMainCamera());

		RayCollision closestCollision;
		if (world.Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
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

void TutorialGame::ConvertTime(float seconds, int& hours, int& minutes, int& remainingSeconds) {
	hours = static_cast<int>(seconds / 3600);
	int remainingTime = static_cast<int>(seconds) % 3600;
	minutes = remainingTime / 60;
	remainingSeconds = remainingTime % 60;
}

void TutorialGame::UpdateSlotString() {
	switch (player->GetSlot()) {
	case 0:
		slotString = "Empty";
		break;
	case 2:
		slotString = "Pickaxe";
		break;
	case 3:
		slotString = "Axe";
		break;
	case 4:
		if (bucket->GetWater())
			slotString = "Bucket(Full)";
		else
			slotString = "Bucket(Empty)";
		break;
	case 5:
		slotString = "Plank";
		break;
	case 6:
		slotString = "Stone";
		break;
	case 7:
		slotString = "Rail";
		break;
	default:
		slotString = "Empty";
		break;
	}
}