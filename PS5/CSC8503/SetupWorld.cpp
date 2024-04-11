#include "Assets.h"
#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "StateGameObject.h"

#include <fstream>

using namespace NCL;
using namespace CSC8503;

void TutorialGame::InitMeshes() {
	std::cout << std::endl << "--------Loading Meshes--------" << std::endl;
	cubeMesh = renderer.LoadMesh("cube.msh");
	sphereMesh = renderer.LoadMesh("sphere.msh");
	charMesh = renderer.LoadMesh("CesiumMan.gltf");
	enemyMesh = renderer.LoadMesh("Role_T.msh");
	bonusMesh = renderer.LoadMesh("apple.msh");
	capsuleMesh = renderer.LoadMesh("capsule.msh");
	//capsuleMesh = renderer->LoadMesh("capsule.msh");
	stationMesh = renderer.LoadMesh("Station.msh");
	carriageMesh = renderer.LoadMesh("Carriage.msh");
	treeMesh = renderer.LoadMesh("Tree.msh");
	rockMesh = renderer.LoadMesh("Rock.msh");
	desertRockMesh = renderer.LoadMesh("DesertRock.msh");
	axeMesh = renderer.LoadMesh("axe.msh");
	pickaxeMesh = renderer.LoadMesh("Pickaxe.msh");
	bucketMesh = renderer.LoadMesh("Bucket_Empty.msh");
	trainMesh = renderer.LoadMesh("Train.msh");
	plankMesh = renderer.LoadMesh("Plank.msh");
	stoneMesh = renderer.LoadMesh("Stone.msh");
	railMesh = renderer.LoadMesh("Rail.msh");
	railTurnMesh = renderer.LoadMesh("RailTurn.msh");
	//assassinMesh = renderer.LoadMesh("Assassin.msh");
	mooseMesh = renderer.LoadMesh("goat.msh");
	robotMesh = renderer.LoadMesh("security.msh");
}

void TutorialGame::InitTextures() {
	std::cout << std::endl << "--------Loading Textures--------" << std::endl;
	basicTex = renderer.LoadTexture("checkerboard.png");
	floorTex = renderer.LoadTexture("grass.jpg");
	carriageTexture = renderer.LoadTexture("CartEmpty_T.png");
	treeTexture = renderer.LoadTexture("Tree.png");
	rockTexture = renderer.LoadTexture("Rock.png");
	desertRockTexture = renderer.LoadTexture("DesertRock.png");
	waterTexture = renderer.LoadTexture("water.jpg");
	//lightTex = renderer->LoadTexture("redstone_lamp_on.png");
	axeTexture = renderer.LoadTexture("Axe_albedo.jpg");
	pickaxeTexture = renderer.LoadTexture("lowpoly_pickaxe_BaseColor.png");
	bucketTexture = renderer.LoadTexture("lambert2_Base_Color.png");
	trainTexture = renderer.LoadTexture("ColourPal.png");
	plankTexture = renderer.LoadTexture("Planks_Diff.png");
	stoneTexture = renderer.LoadTexture("Stone.png");
	railTexture = renderer.LoadTexture("Rail.png");
	railTurnTexture = renderer.LoadTexture("RailTurn.jpg");
	//droneTexture = renderer.LoadTexture("Drone_diff.jpg");

	//stationTex = renderer->LoadGLTexture("Station.png");
}

void TutorialGame::InitAnimations() {
	std::cout << std::endl << "--------Loading Animations--------" << std::endl;
	enemyAnim = new MeshAnimation("Role_T.anm");
	//droneAnim = new MeshAnimation("Drone.anm");

	//maleAnimation = new AnimationObject();
	//maleAnimation->SetAnim1(new MeshAnimation("Idle1.anm"));
	//maleAnimation->SetAnim2(new MeshAnimation("StepForward1.anm"));
	//maleAnimation->SetAnim3(new MeshAnimation("StepLeft1.anm"));
	//maleAnimation->SetAnim4(new MeshAnimation("StepRight1.anm"));
	//maleAnimation->SetAnim5(new MeshAnimation("StepBack1.anm"));
	//maleAnimation->SetAnim6(new MeshAnimation("Angry1.anm"));
	//maleAnimation->SetAnim7(new MeshAnimation("Happy1.anm"));
	//maleAnimation->SetAnim8(new MeshAnimation("Cheer1.anm"));
	//maleAnimation->SetActiveAnim(maleAnimation->GetAnim1());
	//
	//femaleAnimation = new AnimationObject();
	//femaleAnimation->SetAnim1(new MeshAnimation("Idle2.anm"));
	//femaleAnimation->SetAnim2(new MeshAnimation("StepForward2.anm"));
	//femaleAnimation->SetAnim3(new MeshAnimation("StepLeft2.anm"));
	//femaleAnimation->SetAnim4(new MeshAnimation("StepRight2.anm"));
	//femaleAnimation->SetAnim5(new MeshAnimation("StepBack2.anm"));
	//femaleAnimation->SetAnim6(new MeshAnimation("Angry2.anm"));
	//femaleAnimation->SetAnim7(new MeshAnimation("Happy2.anm"));
	//femaleAnimation->SetAnim8(new MeshAnimation("Cheer2.anm"));
	//femaleAnimation->SetActiveAnim(femaleAnimation->GetAnim1());
	//
	//assassinAnimation = new AnimationObject();
	//assassinAnimation->SetAnim1(new MeshAnimation("Assassin.anm"));
	//assassinAnimation->SetAnim2(new MeshAnimation("Assassin.anm"));
	//assassinAnimation->SetAnim3(new MeshAnimation("Assassin.anm"));
	//assassinAnimation->SetAnim4(new MeshAnimation("Assassin.anm"));
	//assassinAnimation->SetAnim5(new MeshAnimation("Assassin.anm"));
	//assassinAnimation->SetActiveAnim(assassinAnimation->GetAnim1());
	//assassinAnimation->SetIdle(false);
	//
	//girlAnimation = new AnimationObject();
	//girlAnimation->SetAnim1(new MeshAnimation("Girl.anm"));
	//girlAnimation->SetAnim2(new MeshAnimation("Girl.anm"));
	//girlAnimation->SetAnim3(new MeshAnimation("Girl.anm"));
	//girlAnimation->SetAnim4(new MeshAnimation("Girl.anm"));
	//girlAnimation->SetAnim5(new MeshAnimation("Girl.anm"));
	//girlAnimation->SetActiveAnim(girlAnimation->GetAnim1());
	//girlAnimation->SetIdle(false);

	//smurfAnimation = new AnimationObject();
	//smurfAnimation->SetAnim1(new MeshAnimation("Smurf.anm"));
	//smurfAnimation->SetAnim2(new MeshAnimation("Smurf.anm"));
	//smurfAnimation->SetAnim3(new MeshAnimation("Smurf.anm"));
	//smurfAnimation->SetAnim4(new MeshAnimation("Smurf.anm"));
	//smurfAnimation->SetAnim5(new MeshAnimation("Smurf.anm"));
	//smurfAnimation->SetActiveAnim(smurfAnimation->GetAnim1());
	//smurfAnimation->SetIdle(false);

	//mooseAnimation = new AnimationObject();
	//mooseAnimation->SetAnim1(new MeshAnimation("Moose.anm"));
	//mooseAnimation->SetAnim2(new MeshAnimation("Moose.anm"));
	//mooseAnimation->SetAnim3(new MeshAnimation("Moose.anm"));
	//mooseAnimation->SetAnim4(new MeshAnimation("Moose.anm"));
	//mooseAnimation->SetAnim5(new MeshAnimation("Moose.anm"));
	//mooseAnimation->SetActiveAnim(mooseAnimation->GetAnim1());
	//
	//robotAnimation = new AnimationObject();
	//robotAnimation->SetAnim1(new MeshAnimation("Robot.anm"));
	//robotAnimation->SetAnim2(new MeshAnimation("Robot.anm"));
	//robotAnimation->SetAnim3(new MeshAnimation("Robot.anm"));
	//robotAnimation->SetAnim4(new MeshAnimation("Robot.anm"));
	//robotAnimation->SetAnim5(new MeshAnimation("Robot.anm"));
	//robotAnimation->SetActiveAnim(robotAnimation->GetAnim1());
	//
	//droneAnimation = new AnimationObject();
	//droneAnimation->SetAnim1(new MeshAnimation("Drone.anm"));
	//droneAnimation->SetAnim2(new MeshAnimation("Drone.anm"));
	//droneAnimation->SetAnim3(new MeshAnimation("Drone.anm"));
	//droneAnimation->SetAnim4(new MeshAnimation("Drone.anm"));
	//droneAnimation->SetAnim5(new MeshAnimation("Drone.anm"));
	//droneAnimation->SetActiveAnim(droneAnimation->GetAnim1());
}

void TutorialGame::InitShaders() {
	std::cout << std::endl << "--------Loading Shader Groups--------" << std::endl;
	basicShader = renderer.LoadShader("scene.vert", "scene.frag");
}

void TutorialGame::AddSceneToWorld(int level)
{
	int nodeSize;
	int gridWidth;
	int gridHeight;
	std::ifstream infile;
	switch (level) {
	case 1:
		infile = std::ifstream(Assets::DATADIR + "map1.txt");
		infile >> nodeSize;
		infile >> gridWidth;
		infile >> gridHeight;
		navGrid = new NavigationGrid("map1.txt");
		break;
	case 2:
		infile = std::ifstream(Assets::DATADIR + "map2.txt");
		infile >> nodeSize;
		infile >> gridWidth;
		infile >> gridHeight;
		navGrid = new NavigationGrid("map2.txt");
		break;
	case 3:
		infile = std::ifstream(Assets::DATADIR + "map3.txt");
		infile >> nodeSize;
		infile >> gridWidth;
		infile >> gridHeight;
		navGrid = new NavigationGrid("map3.txt");
		break;
	case 4:
		infile = std::ifstream(Assets::DATADIR + "map4.txt");
		infile >> nodeSize;
		infile >> gridWidth;
		infile >> gridHeight;
		navGrid = new NavigationGrid("map4.txt");
		break;
	case 5:
		infile = std::ifstream(Assets::DATADIR + "map5.txt");
		infile >> nodeSize;
		infile >> gridWidth;
		infile >> gridHeight;
		navGrid = new NavigationGrid("map5.txt");
		break;
	default:
		break;
	}

	GridNode* nodes = new GridNode[gridWidth * gridHeight];
	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x)
		{
			GridNode& n = nodes[(gridWidth * y) + x];
			char type = 0;
			infile >> type;
			n.type = type;
			n.position = Vector3((float)(x * nodeSize), 7, (float)(y * nodeSize));
			Vector3 position = Vector3((float)(x * nodeSize), 7, (float)(y * nodeSize));
			if (type == '1')AddDesertRockToWorld(n.position);
			if (type == '2')AddTreeToWorld(n.position + Vector3(0, 5.5f, 0));
			if (type == '3')AddRockToWorld(n.position + Vector3(0, -1.5f, 0));
			if (type == '4')AddWaterToWorld(n.position + Vector3(0, -1.5f, 0));
			if (type == '5')AddStationToWorld(n.position + Vector3(0, -2.5f, 6));
			if (type == '6') {
				RailObject* rail = new RailObject(&world);
				rail = AddRailToWorld(n.position + Vector3(0, -2.5f, 0), true);
				navGrid->GetGridNode((gridWidth * y) + x).SetRail(rail);
			}
		}
	}
	return;
}

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(155, 2, 100);
	AABBVolume* volume = new AABBVolume(Vector3(0.1f, 0.1f, 0.1f));
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(Vector3(310, 4, 200))
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, floorTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();
	floor->GetPhysicsObject()->SetResolve(false);

	world.AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world.AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2.0f);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(cube);

	return cube;
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

GameObject* TutorialGame::AddPlayer0ToWorld(const Vector3& position) {
	float meshSize = 1.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();
	SphereVolume* volume = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world.AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->GetRenderObject()->SetAnimation(*enemyAnim);

	world.AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world.AddGameObject(apple);

	return apple;
}

TrainObject* TutorialGame::AddTrainToWorld(const Vector3& position) {
	TrainObject* train = new TrainObject(&world);

	AABBVolume* volume = new AABBVolume(Vector3(5, 15, 5));
	train->SetBoundingVolume((CollisionVolume*)volume);

	train->GetTransform()
		.SetScale(Vector3(10, 10, 10))
		.SetPosition(train->FindGrid(position));

	train->SetInitPos(position);
	train->SetRenderObject(new RenderObject(&train->GetTransform(), trainMesh, trainTexture, basicShader));
	train->SetPhysicsObject(new PhysicsObject(&train->GetTransform(), train->GetBoundingVolume()));

	train->GetPhysicsObject()->SetInverseMass(0);
	train->GetPhysicsObject()->InitCubeInertia();
	train->GetPhysicsObject()->SetChannel(1);

	train->UploadAssets(carriageMesh, carriageTexture, basicShader);

	world.AddGameObject(train);

	return train;
}

GameObject* TutorialGame::AddStationToWorld(const Vector3& position) {
	GameObject* station = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(7.5f, 7.5f, 7.5f));
	station->SetBoundingVolume((CollisionVolume*)volume);

	station->GetTransform()
		.SetScale(Vector3(15, 15, 15))
		.SetPosition(position);

	station->SetRenderObject(new RenderObject(&station->GetTransform(), stationMesh, stationTexture, basicShader));
	station->SetPhysicsObject(new PhysicsObject(&station->GetTransform(), station->GetBoundingVolume()));

	station->GetPhysicsObject()->SetInverseMass(0);
	station->GetPhysicsObject()->InitSphereInertia();

	station->SetTypeID(114514);

	world.AddGameObject(station);

	return station;
}

//GameObject* TutorialGame::AddTestingLightToWorld(const Vector3& position, const Vector4& colour) {
	//GameObject* cube = new GameObject();
	//
	//AABBVolume* volume = new AABBVolume(Vector3(0.5, 0.5, 0.5));
	//cube->SetBoundingVolume((CollisionVolume*)volume);
	//
	//cube->GetTransform()
	//    .SetPosition(position)
	//    .SetScale(Vector3(1, 1, 1));
	//
	//cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, lightTex, bumpShader));
	//cube->GetRenderObject()->SetBumpTexture(lightBumpTex);
	//cube->GetRenderObject()->SetSpecTexture(lightSpecTex);
	//cube->GetRenderObject()->SetColour(colour);
	//cube->GetRenderObject()->SetEmissive(true);
	//
	//cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	//
	//cube->GetPhysicsObject()->SetInverseMass(1);
	//cube->GetPhysicsObject()->InitCubeInertia();
	//
	//world->AddGameObject(cube);
	//
	//return cube;
//}

PlayerObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	PlayerObject* player = new PlayerObject("Player");
	AABBVolume* volume = new AABBVolume(Vector3(1.5f, 1.5f, 1.5f));
	player->SetBoundingVolume((CollisionVolume*)volume);

	player->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(3, 3, 3));

	player->SetRenderObject(new RenderObject(&player->GetTransform(), enemyMesh, nullptr, basicShader));
	player->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));

	player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));
	player->GetPhysicsObject()->SetInverseMass(1);

	player->GetRenderObject()->SetAnimation(*enemyAnim);

	world.AddGameObject(player);

	return player;
}

TreeObject* TutorialGame::AddTreeToWorld(const Vector3& position) {
	float meshSize = 2.5f;
	float inverseMass = 0;

	TreeObject* tree = new TreeObject();
	tree->Setscale(meshSize);
	AABBVolume* volume = new AABBVolume(Vector3(1.5f, 4, 1.5f) * tree->Getscale());
	tree->SetBoundingVolume((CollisionVolume*)volume);
	tree->GetTransform()
		.SetScale(Vector3(tree->Getscale(), tree->Getscale(), tree->Getscale()))
		.SetPosition(tree->FindGrid(position));

	tree->SetRenderObject(new RenderObject(&tree->GetTransform(), treeMesh, treeTexture, basicShader));// todo can change capsule
	tree->SetPhysicsObject(new PhysicsObject(&tree->GetTransform(), tree->GetBoundingVolume()));

	tree->GetPhysicsObject()->SetInverseMass(inverseMass);
	tree->GetPhysicsObject()->InitSphereInertia();
	tree->GetPhysicsObject()->SetChannel(1);

	world.AddGameObject(tree);

	return tree;
}

RockObject* TutorialGame::AddRockToWorld(const Vector3& position) {
	float meshSize = 2.5f;
	float inverseMass = 0;

	RockObject* rock = new RockObject();
	rock->Setscale(meshSize);
	AABBVolume* volume = new AABBVolume(Vector3(2, 1, 2) * rock->Getscale());
	rock->SetBoundingVolume((CollisionVolume*)volume);
	rock->GetTransform()
		.SetScale(Vector3(rock->Getscale(), rock->Getscale(), rock->Getscale()))
		.SetPosition(rock->FindGrid(position));

	rock->SetRenderObject(new RenderObject(&rock->GetTransform(), rockMesh, rockTexture, basicShader));// todo can change capsule
	rock->SetPhysicsObject(new PhysicsObject(&rock->GetTransform(), rock->GetBoundingVolume()));

	rock->GetPhysicsObject()->SetInverseMass(inverseMass);
	rock->GetPhysicsObject()->InitSphereInertia();
	rock->GetPhysicsObject()->SetChannel(1);

	world.AddGameObject(rock);

	return rock;
}

GameObject* TutorialGame::AddDesertRockToWorld(const Vector3& position) {
	float inverseMass = 0;

	GameObject* rock = new GameObject();
	AABBVolume* volume = new AABBVolume(Vector3(3.5f, 3, 3.5f));
	rock->SetBoundingVolume((CollisionVolume*)volume);
	rock->GetTransform()
		.SetScale(Vector3(3, 3, 3))
		.SetPosition(rock->FindGrid(position));

	rock->SetName("Desert Rock");

	rock->SetRenderObject(new RenderObject(&rock->GetTransform(), desertRockMesh, desertRockTexture, basicShader));// todo can change capsule
	rock->SetPhysicsObject(new PhysicsObject(&rock->GetTransform(), rock->GetBoundingVolume()));

	rock->GetPhysicsObject()->SetInverseMass(inverseMass);
	rock->GetPhysicsObject()->InitSphereInertia();

	rock->SetTypeID(12345);

	world.AddGameObject(rock);

	return rock;
}

GameObject* TutorialGame::AddWaterToWorld(const Vector3& position) {
	GameObject* cube = new GameObject();
	AABBVolume* volume = new AABBVolume(Vector3(5, 5, 5));
	cube->SetBoundingVolume((CollisionVolume*)volume);
	cube->GetTransform()
		.SetScale(Vector3(10, 2, 10))
		.SetPosition(cube->FindGrid(position));

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, waterTexture, basicShader));// todo can change capsule
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitSphereInertia();
	cube->GetPhysicsObject()->SetChannel(1);

	cube->SetTypeID(10000);

	world.AddGameObject(cube);

	return cube;
}

AxeObject* TutorialGame::AddAxeToWorld(const Vector3& position) {
	AxeObject* axe = new AxeObject(&world, "Axe");

	AABBVolume* volume = new AABBVolume(Vector3(0.5f, 4, 0.5f));
	axe->SetBoundingVolume((CollisionVolume*)volume);

	axe->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(1, 1, 1));

	//axe->SetPlayer(player);

	axe->SetRenderObject(new RenderObject(&axe->GetTransform(), axeMesh, axeTexture, basicShader));
	axe->SetPhysicsObject(new PhysicsObject(&axe->GetTransform(), axe->GetBoundingVolume()));

	axe->GetPhysicsObject()->SetResolve(false);
	axe->GetPhysicsObject()->SetInverseMass(1);
	axe->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(axe);

	return axe;
}

PickaxeObject* TutorialGame::AddPickaxeToWorld(const Vector3& position) {
	PickaxeObject* pickaxe = new PickaxeObject(&world, "Pickaxe");

	AABBVolume* volume = new AABBVolume(Vector3(2, 2, 2));
	pickaxe->SetBoundingVolume((CollisionVolume*)volume);

	pickaxe->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(4, 4, 4));

	//pickaxe->SetPlayer(player);

	pickaxe->SetRenderObject(new RenderObject(&pickaxe->GetTransform(), pickaxeMesh, pickaxeTexture, basicShader));
	pickaxe->SetPhysicsObject(new PhysicsObject(&pickaxe->GetTransform(), pickaxe->GetBoundingVolume()));

	pickaxe->GetPhysicsObject()->SetResolve(false);
	pickaxe->GetPhysicsObject()->SetInverseMass(1);
	pickaxe->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(pickaxe);

	return pickaxe;
}

BucketObject* TutorialGame::AddBucketToWorld(const Vector3& position) {
	BucketObject* bucket = new BucketObject(&world, "bucket");

	AABBVolume* volume = new AABBVolume(Vector3(2, 2, 2));
	bucket->SetBoundingVolume((CollisionVolume*)volume);

	bucket->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(4, 4, 4));

	//bucket->SetPlayer(player);

	bucket->SetRenderObject(new RenderObject(&bucket->GetTransform(), bucketMesh, bucketTexture, basicShader));
	bucket->SetPhysicsObject(new PhysicsObject(&bucket->GetTransform(), bucket->GetBoundingVolume()));

	bucket->GetPhysicsObject()->SetResolve(false);
	bucket->GetPhysicsObject()->SetInverseMass(1);
	bucket->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(bucket);

	return bucket;
}

//PlankObject* TutorialGame::AddPlankToWorld(const Vector3& position, bool network, int id) {
	//PlankObject* plank = new PlankObject(world);
	//
	//AABBVolume* volume = new AABBVolume(Vector3(2, 2, 2));
	//plank->SetBoundingVolume((CollisionVolume*)volume);
	//
	////plank->SetPlayer(player);
	//
	//plank->GetTransform()
	//    .SetPosition(plank->FindNearestGridCenter(position))
	//    .SetScale(Vector3(4, 4, 4));
	//
	//plank->SetRenderObject(new RenderObject(&plank->GetTransform(), plankMesh, plankTex, bumpShader));
	//plank->GetRenderObject()->SetBumpTexture(plankBumpTex);
	//plank->SetPhysicsObject(new PhysicsObject(&plank->GetTransform(), plank->GetBoundingVolume()));
	//
	//plank->GetPhysicsObject()->SetGravity(false);
	//plank->GetPhysicsObject()->SetResolve(false);
	//plank->GetPhysicsObject()->SetInverseMass(1);
	//plank->GetPhysicsObject()->InitCubeInertia();
	//
	//world->AddGameObject(plank);
	//
	//if (!network)
	//    plank->SetNetworkObject(new NetworkObject(*plank, plank->GetWorldID() + 1000));
	//else
	//    plank->SetNetworkObject(new NetworkObject(*plank, id));
	//
	//return plank;
//}

//StoneObject* TutorialGame::AddStoneToWorld(const Vector3& position, bool network, int id) {
	//StoneObject* stone = new StoneObject(world);
	//
	//SphereVolume* volume = new SphereVolume(2);
	//stone->SetBoundingVolume((CollisionVolume*)volume);
	//
	////stone->SetPlayer(player);
	//
	//stone->GetTransform()
	//    .SetPosition(stone->FindNearestGridCenter(position))
	//    .SetScale(Vector3(4, 4, 4));
	//
	//stone->SetRenderObject(new RenderObject(&stone->GetTransform(), stoneMesh, stoneTex, bumpShader));
	//stone->GetRenderObject()->SetBumpTexture(stoneBumpTex);
	//stone->SetPhysicsObject(new PhysicsObject(&stone->GetTransform(), stone->GetBoundingVolume()));
	//
	//stone->GetPhysicsObject()->SetGravity(false);
	//stone->GetPhysicsObject()->SetResolve(false);
	//stone->GetPhysicsObject()->SetInverseMass(1);
	//stone->GetPhysicsObject()->InitCubeInertia();
	//
	//world->AddGameObject(stone);
	//
	//if (!network)
	//    stone->SetNetworkObject(new NetworkObject(*stone, stone->GetWorldID() + 2000));
	//else
	//    stone->SetNetworkObject(new NetworkObject(*stone, id));
	//
	//return stone;
//}

PlankObject* TutorialGame::AddPlankToWorld(const Vector3& position) {
	PlankObject* plank = new PlankObject(&world);

	AABBVolume* volume = new AABBVolume(Vector3(2, 2, 2));
	plank->SetBoundingVolume((CollisionVolume*)volume);

	plank->SetPlayer(player);

	plank->GetTransform()
		.SetPosition(plank->FindNearestGridCenter(position))
		.SetScale(Vector3(4, 4, 4));

	plank->SetRenderObject(new RenderObject(&plank->GetTransform(), plankMesh, plankTexture, basicShader));
	plank->SetPhysicsObject(new PhysicsObject(&plank->GetTransform(), plank->GetBoundingVolume()));

	plank->GetPhysicsObject()->SetResolve(false);
	plank->GetPhysicsObject()->SetInverseMass(1);
	plank->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(plank);

	return plank;
}

StoneObject* TutorialGame::AddStoneToWorld(const Vector3& position) {
	StoneObject* stone = new StoneObject(&world);

	AABBVolume* volume = new AABBVolume(Vector3(2, 2, 2));
	stone->SetBoundingVolume((CollisionVolume*)volume);

	stone->SetPlayer(player);

	stone->GetTransform()
		.SetPosition(stone->FindNearestGridCenter(position))
		.SetScale(Vector3(4, 4, 4));

	stone->SetRenderObject(new RenderObject(&stone->GetTransform(), stoneMesh, stoneTexture, basicShader));
	stone->SetPhysicsObject(new PhysicsObject(&stone->GetTransform(), stone->GetBoundingVolume()));

	stone->GetPhysicsObject()->SetResolve(false);
	stone->GetPhysicsObject()->SetInverseMass(1);
	stone->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(stone);

	return stone;
}

RailObject* TutorialGame::AddRailToWorld(const Vector3& position, bool placed)
{
	RailObject* rail = new RailObject(&world);

	AABBVolume* volume = new AABBVolume(Vector3(2, 2, 2));
	rail->SetBoundingVolume((CollisionVolume*)volume);

	//rail->SetPlayer(player);

	rail->SetRenderObject(new RenderObject(&rail->GetTransform(), railMesh, railTexture, basicShader));
	rail->SetPhysicsObject(new PhysicsObject(&rail->GetTransform(), rail->GetBoundingVolume()));

	rail->GetPhysicsObject()->SetGravity(false);
	rail->GetPhysicsObject()->SetResolve(false);

	rail->UploadAssets(railTurnMesh, railTurnTexture);

	if (!placed) {
		rail->GetTransform()
			.SetPosition(position)
			.SetScale(Vector3(4, 4, 4));

		rail->GetPhysicsObject()->SetInverseMass(1);
		rail->GetPhysicsObject()->InitCubeInertia();

		world.AddGameObject(rail);

		return rail;
	}
	else {
		rail->GetTransform()
			.SetPosition(rail->FindGrid(position))
			.SetScale(Vector3(10, 10, 10));

		rail->GetPhysicsObject()->SetInverseMass(0);
		rail->GetPhysicsObject()->InitCubeInertia();
		rail->SetInCarriage(false);
		rail->SetPlaced(true);
		rail->SetFlag1(true);

		rail->SetDirection(3);

		world.AddGameObject(rail);

		return rail;
	}
}

AnimalObject* TutorialGame::AddMooseToWorld(const Vector3& position) {
	AnimalObject* moose = new AnimalObject(navGrid, position, &world);
	AABBVolume* volume = new AABBVolume(Vector3(1.5, 1.5, 1.5));
	moose->SetBoundingVolume((CollisionVolume*)volume);

	moose->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(3, 3, 3));

	moose->SetRenderObject(new RenderObject(&moose->GetTransform(), mooseMesh, nullptr, basicShader));
	moose->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));

	moose->SetPhysicsObject(new PhysicsObject(&moose->GetTransform(), moose->GetBoundingVolume()));
	moose->GetPhysicsObject()->SetInverseMass(1);
	moose->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(moose);

	GameObject* detSphere = AddDetectionSphereToWorld(position, 25.0f, moose);

	return moose;
}

DetectionSphereObject* TutorialGame::AddDetectionSphereToWorld(const Vector3& position, float radius, AnimalObject* animal) {
	DetectionSphereObject* sphere = new DetectionSphereObject(animal);

	Vector3 sphereSize = Vector3(radius, radius, radius);
	AABBVolume* volume = new AABBVolume(sphereSize);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	//sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(10.0f);
	sphere->GetPhysicsObject()->InitSphereInertia();
	//sphere->GetRenderObject()->SetColour(Vector4(0, 0, 0, 0));
	world.AddGameObject(sphere);

	((CollisionVolume*)sphere->GetBoundingVolume())->SetIsTrigger(true);
	PositionConstraint* constraint = new PositionConstraint(animal, sphere, 0.01f);
	world.AddConstraint(constraint);

	return sphere;
}

RobotObject* TutorialGame::AddRobotToWorld(const Vector3& position) {
	RobotObject* robot = new RobotObject(navGrid, position);
	AABBVolume* volume = new AABBVolume(Vector3(2, 4, 2));
	robot->SetBoundingVolume((CollisionVolume*)volume);

	robot->GetTransform()
		.SetPosition(position)
		.SetScale(Vector3(4, 4, 4));

	robot->SetRenderObject(new RenderObject(&robot->GetTransform(), mooseMesh, nullptr, basicShader));
	robot->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

	robot->SetPhysicsObject(new PhysicsObject(&robot->GetTransform(), robot->GetBoundingVolume()));
	robot->GetPhysicsObject()->SetInverseMass(1);
	robot->GetPhysicsObject()->InitCubeInertia();

	world.AddGameObject(robot);

	return robot;
}

//DroneObject* TutorialGame::AddDroneToWorld(const Vector3& position) {
	//DroneObject* drone = new DroneObject();
	//AABBVolume* volume = new AABBVolume(Vector3(1.5, 1.5, 1.5));
	//drone->SetBoundingVolume((CollisionVolume*)volume);
	//
	//drone->GetTransform()
	//    .SetPosition(position)
	//    .SetScale(Vector3(3, 3, 3));
	//
	//drone->SetRenderObject(new RenderObject(&drone->GetTransform(), droneMesh, nullptr, skinningPerPixelShader, 3));
	//drone->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
	//drone->GetRenderObject()->SetAnimationObject(droneAnimation);
	//drone->GetRenderObject()->SetTextures(droneTextures);
	//
	//drone->SetPhysicsObject(new PhysicsObject(&drone->GetTransform(), drone->GetBoundingVolume()));
	//drone->GetPhysicsObject()->SetInverseMass(1);
	//drone->GetPhysicsObject()->InitCubeInertia();
	//
	//world->AddGameObject(drone);
	//
	//return drone;
//}

GameObject* TutorialGame::AddPadToWorld() {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(3, 0.1f, 3);
	SphereVolume* volume = new SphereVolume(0.01f);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(Vector3());

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(0);
	sphere->GetPhysicsObject()->InitSphereInertia();
	sphere->GetPhysicsObject()->SetResolve(false);
	sphere->GetRenderObject()->SetColour(Vector4());
	world.AddGameObject(sphere);

	return sphere;
}