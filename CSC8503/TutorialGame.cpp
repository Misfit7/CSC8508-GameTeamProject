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

    InitialiseAssets();
    BridgeConstraintTest();

    instance = this;
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes,
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
    std::cout << std::endl << "--------Initialising Assets--------" << std::endl;

    std::cout << std::endl << "--------Loading Meshes--------" << std::endl;
    cubeMesh = renderer->LoadMesh("cube.msh");
    sphereMesh = renderer->LoadMesh("sphere.msh");
    charMesh = renderer->LoadMesh("goat.msh");
    enemyMesh = renderer->LoadMesh("Keeper.msh");
    bonusMesh = renderer->LoadMesh("apple.msh");
    capsuleMesh = renderer->LoadMesh("capsule.msh");
    trainMesh = renderer->LoadOBJMesh("Train.obj");
    creeperMesh = renderer->LoadOBJMesh("Creeper.obj");
    maleMesh = renderer->LoadMesh("Male_Guard.msh");
    femaleMesh = renderer->LoadMesh("Female_Guard.msh");
    assassinMesh = renderer->LoadMesh("Assassin.msh");
    girlMesh = renderer->LoadMesh("Girl.msh");
    smurfMesh = renderer->LoadMesh("Smurf.msh");

    meshes.push_back(maleMesh);
    meshes.push_back(femaleMesh);
    meshes.push_back(assassinMesh);
    meshes.push_back(girlMesh);
    meshes.push_back(smurfMesh);

    std::cout << std::endl << "--------Loading Textures--------" << std::endl;
    basicTex = renderer->LoadTexture("checkerboard.png");
    floorTex = renderer->LoadTexture("checkerboard.png");
    trainTex = renderer->LoadTexture("Train.jpg");
    lightTex = renderer->LoadTexture("redstone_lamp_on.png");

    lightBumpTex = renderer->LoadTexture("redstone_lamp_on_n.png");

    lightSpecTex = renderer->LoadTexture("redstone_lamp_on_s.png");

    InitMaterials();
    InitAnimations();

    std::cout << std::endl << "--------Loading Shaders--------" << std::endl;
    basicDayShader = renderer->LoadShader("PerPixel.vert", "PerPixelScene.frag");
    bumpDayShader = renderer->LoadShader("Bump.vert", "BumpScene.frag");
    specDayShader = renderer->LoadShader("Bump.vert", "SpecScene.frag");
    skinningPerPixelDayShader = renderer->LoadShader("SkinningPerPixel.vert", "SkinningPerPixelScene.frag");
    skinningBumpDayShader = renderer->LoadShader("SkinningBump.vert", "SkinningBumpScene.frag");

    basicNightShader = renderer->LoadShader("PerPixel.vert", "PerPixelBuffer.frag");
    bumpNightShader = renderer->LoadShader("Bump.vert", "BumpBuffer.frag");
    specNightShader = renderer->LoadShader("Bump.vert", "SpecBuffer.frag");
    skinningPerPixelNightShader = renderer->LoadShader("SkinningPerPixel.vert", "SkinningPerPixelBuffer.frag");
    skinningBumpNightShader = renderer->LoadShader("SkinningBump.vert", "SkinningBumpBuffer.frag");

    basicShader = new ShaderGroup(basicDayShader, basicNightShader);
    bumpShader = new ShaderGroup(bumpDayShader, bumpNightShader);
    specShader = new ShaderGroup(specDayShader, specNightShader);
    skinningPerPixelShader = new ShaderGroup(skinningPerPixelDayShader, skinningPerPixelNightShader);
    skinningBumpShader = new ShaderGroup(skinningBumpDayShader, skinningBumpNightShader);

    shaders.push_back(skinningBumpShader);
    shaders.push_back(skinningBumpShader);
    shaders.push_back(skinningPerPixelShader);
    shaders.push_back(skinningBumpShader);
    shaders.push_back(skinningBumpShader);

    InitCamera();
    InitWorld();
}

void TutorialGame::InitMaterials() {
    std::cout << std::endl << "--------Loading Materials--------" << std::endl;
    maleMaterial = new MeshMaterial("Male_Guard.mat");
    for (int i = 0; i < maleMesh->GetSubMeshCount(); ++i) {
        const MeshMaterialEntry* matEntry =
            maleMaterial->GetMaterialForLayer(i);

        const string* filename = nullptr;
        matEntry->GetEntry("Diffuse", &filename);
        string path = Assets::TEXTUREDIR + *filename;
        GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        maleTextures.emplace_back(texID);

        matEntry->GetEntry("Bump", &filename);
        string path2 = Assets::TEXTUREDIR + *filename;
        GLuint texID2 = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        maleBumpTextures.emplace_back(texID2);
    }

    femaleMaterial = new MeshMaterial("Female_Guard.mat");
    for (int i = 0; i < femaleMesh->GetSubMeshCount(); ++i) {
        const MeshMaterialEntry* matEntry =
            femaleMaterial->GetMaterialForLayer(i);

        const string* filename = nullptr;
        matEntry->GetEntry("Diffuse", &filename);
        string path = Assets::TEXTUREDIR + *filename;
        GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        femaleTextures.emplace_back(texID);

        matEntry->GetEntry("Bump", &filename);
        string path2 = Assets::TEXTUREDIR + *filename;
        GLuint texID2 = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        femaleBumpTextures.emplace_back(texID2);
    }

    assassinMaterial = new MeshMaterial("Assassin.mat");
    for (int i = 0; i < assassinMesh->GetSubMeshCount(); ++i) {
        const MeshMaterialEntry* matEntry =
            assassinMaterial->GetMaterialForLayer(i);

        const string* filename = nullptr;
        matEntry->GetEntry("Diffuse", &filename);
        string path = Assets::TEXTUREDIR + *filename;
        GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        assassinTextures.emplace_back(texID);
    }
    vector<GLuint> assassinBumpTextures;

    girlMaterial = new MeshMaterial("Girl.mat");
    for (int i = 0; i < girlMesh->GetSubMeshCount(); ++i) {
        const MeshMaterialEntry* matEntry =
            girlMaterial->GetMaterialForLayer(i);

        const string* filename = nullptr;
        matEntry->GetEntry("Diffuse", &filename);
        string path = Assets::TEXTUREDIR + *filename;
        GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        girlTextures.emplace_back(texID);

        matEntry->GetEntry("Bump", &filename);
        string path2 = Assets::TEXTUREDIR + *filename;
        GLuint texID2 = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        girlBumpTextures.emplace_back(texID2);
    }

    smurfMaterial = new MeshMaterial("Smurf.mat");
    for (int i = 0; i < smurfMesh->GetSubMeshCount(); ++i) {
        const MeshMaterialEntry* matEntry =
            smurfMaterial->GetMaterialForLayer(i);

        const string* filename = nullptr;
        matEntry->GetEntry("Diffuse", &filename);
        string path = Assets::TEXTUREDIR + *filename;
        GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        smurfTextures.emplace_back(texID);

        matEntry->GetEntry("Bump", &filename);
        string path2 = Assets::TEXTUREDIR + *filename;
        GLuint texID2 = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        smurfBumpTextures.emplace_back(texID2);
    }

    textures.push_back(maleTextures);
    textures.push_back(femaleTextures);
    textures.push_back(assassinTextures);
    textures.push_back(girlTextures);
    textures.push_back(smurfTextures);

    bumpTextures.push_back(maleBumpTextures);
    bumpTextures.push_back(femaleBumpTextures);
    bumpTextures.push_back(assassinBumpTextures);
    bumpTextures.push_back(girlBumpTextures);
    bumpTextures.push_back(smurfBumpTextures);
}

void TutorialGame::InitAnimations() {
    std::cout << std::endl << "--------Loading Animations--------" << std::endl;
    maleAnimation = new AnimationObject();
    maleAnimation->SetAnim1(new MeshAnimation("Idle1.anm"));
    maleAnimation->SetAnim2(new MeshAnimation("StepForward1.anm"));
    maleAnimation->SetAnim3(new MeshAnimation("StepLeft1.anm"));
    maleAnimation->SetAnim4(new MeshAnimation("StepRight1.anm"));
    maleAnimation->SetAnim5(new MeshAnimation("StepBack1.anm"));
    maleAnimation->SetAnim6(new MeshAnimation("Angry1.anm"));
    maleAnimation->SetAnim7(new MeshAnimation("Happy1.anm"));
    maleAnimation->SetAnim8(new MeshAnimation("Cheer1.anm"));
    maleAnimation->SetActiveAnim(maleAnimation->GetAnim1());

    femaleAnimation = new AnimationObject();
    femaleAnimation->SetAnim1(new MeshAnimation("Idle2.anm"));
    femaleAnimation->SetAnim2(new MeshAnimation("StepForward2.anm"));
    femaleAnimation->SetAnim3(new MeshAnimation("StepLeft2.anm"));
    femaleAnimation->SetAnim4(new MeshAnimation("StepRight2.anm"));
    femaleAnimation->SetAnim5(new MeshAnimation("StepBack2.anm"));
    femaleAnimation->SetAnim6(new MeshAnimation("Angry2.anm"));
    femaleAnimation->SetAnim7(new MeshAnimation("Happy2.anm"));
    femaleAnimation->SetAnim8(new MeshAnimation("Cheer2.anm"));
    femaleAnimation->SetActiveAnim(femaleAnimation->GetAnim1());

    assassinAnimation = new AnimationObject();
    assassinAnimation->SetAnim1(new MeshAnimation("Assassin.anm"));
    assassinAnimation->SetAnim2(new MeshAnimation("Assassin.anm"));
    assassinAnimation->SetAnim3(new MeshAnimation("Assassin.anm"));
    assassinAnimation->SetAnim4(new MeshAnimation("Assassin.anm"));
    assassinAnimation->SetAnim5(new MeshAnimation("Assassin.anm"));
    assassinAnimation->SetActiveAnim(assassinAnimation->GetAnim1());
    assassinAnimation->SetIdle(false);

    girlAnimation = new AnimationObject();
    girlAnimation->SetAnim1(new MeshAnimation("Girl.anm"));
    girlAnimation->SetAnim2(new MeshAnimation("Girl.anm"));
    girlAnimation->SetAnim3(new MeshAnimation("Girl.anm"));
    girlAnimation->SetAnim4(new MeshAnimation("Girl.anm"));
    girlAnimation->SetAnim5(new MeshAnimation("Girl.anm"));
    girlAnimation->SetActiveAnim(girlAnimation->GetAnim1());
    girlAnimation->SetIdle(false);

    smurfAnimation = new AnimationObject();
    smurfAnimation->SetAnim1(new MeshAnimation("Smurf.anm"));
    smurfAnimation->SetAnim2(new MeshAnimation("Smurf.anm"));
    smurfAnimation->SetAnim3(new MeshAnimation("Smurf.anm"));
    smurfAnimation->SetAnim4(new MeshAnimation("Smurf.anm"));
    smurfAnimation->SetAnim5(new MeshAnimation("Smurf.anm"));
    smurfAnimation->SetActiveAnim(smurfAnimation->GetAnim1());
    smurfAnimation->SetIdle(false);

    animations.push_back(maleAnimation);
    animations.push_back(femaleAnimation);
    animations.push_back(assassinAnimation);
    animations.push_back(girlAnimation);
    animations.push_back(smurfAnimation);
}

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
        world->GetMainCamera().UpdateCamera(dt);
    }
    if (lockedObject != nullptr) {
        Vector3 objPos = lockedObject->GetTransform().GetPosition();
        Vector3 camPos = objPos + lockedOffset;

        Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

        Matrix4 modelMat = temp.Inverse();

        Quaternion q(modelMat);
        Vector3 angles = q.ToEuler(); //nearly there now!

        world->GetMainCamera().SetPosition(camPos);
        world->GetMainCamera().SetPitch(angles.x);
        world->GetMainCamera().SetYaw(angles.y);
        world->GetMainCamera().SetPitch(-90);
        world->GetMainCamera().SetYaw(0);
    }

    UpdateKeys();
    audio->UpdateKeys();

    if (useGravity) {
        Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
    }
    else {
        Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
    }
    Debug::DrawLine(Vector3(0, 0, 0), Vector3(100, 0, 0), Debug::RED);
    Debug::DrawLine(Vector3(0, 0, 0), Vector3(0, 100, 0), Debug::GREEN);
    Debug::DrawLine(Vector3(0, 0, 0), Vector3(0, 0, 100), Debug::BLUE);
    RayCollision closestCollision;
    if (Window::GetKeyboard()->KeyPressed(KeyCodes::K) && selectionObject) {
        Vector3 rayPos;
        Vector3 rayDir;

        rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

        rayPos = selectionObject->GetTransform().GetPosition();

        Ray r = Ray(rayPos, rayDir);

        if (world->Raycast(r, closestCollision, true, selectionObject)) {
            if (objClosest) {
                objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
            }
            objClosest = (GameObject*)closestCollision.node;

            objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
        }
    }

    Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

    SelectObject();
    MoveSelectedObject();

    world->UpdateWorld(dt);
    renderer->Update(dt);
    physics->Update(dt);

    if (testStateObject) {
        //std::cout<<"debug"<<std::endl;
        testStateObject->Update(dt);
    }
    if (trainObject) {
        trainObject->Update(dt);
    }

    renderer->Render();
    Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateKeys() {
    if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
        InitWorld(); //We can reset the simulation at any time with F1
        selectionObject = nullptr;
    }

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
        InitCamera(); //F2 will reset the camera to a specific default place
    }

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::F3)) {
        renderer->ToggleNight();
    }

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::F4)) {
        renderer->ToggleProcess();
    }

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::G)) {
        useGravity = !useGravity; //Toggle gravity!
        physics->UseGravity(useGravity);
    }
    //Running certain physics updates in a consistent order might cause some
    //bias in the calculations - the same objects might keep 'winning' the constraint
    //allowing the other one to stretch too much etc. Shuffling the order so that it
    //is random every frame can help reduce such bias.
    if (Window::GetKeyboard()->KeyPressed(KeyCodes::F9)) {
        world->ShuffleConstraints(true);
    }
    if (Window::GetKeyboard()->KeyPressed(KeyCodes::F10)) {
        world->ShuffleConstraints(false);
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
    else {
        DebugObjectMovement();
    }
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

void TutorialGame::InitCamera() {
    world->GetMainCamera().SetNearPlane(0.1f);
    world->GetMainCamera().SetFarPlane(500.0f);
    world->GetMainCamera().SetPitch(-15.0f);
    world->GetMainCamera().SetYaw(315.0f);
    world->GetMainCamera().SetPosition(Vector3(-60, 40, 60));
    lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
    std::cout << std::endl << "--------Initialising Game Objects--------" << std::endl;
    world->ClearAndErase();
    physics->Clear();

    testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));

    //InitMixedGridWorld(15, 15, 3.5f, 3.5f);
    //AddSceneToWorld();
    InitGameExamples();
    InitDefaultFloor();

}

void TutorialGame::AddSceneToWorld()
{
    int nodeSize;
    int gridWidth;
    int gridHeight;
    std::ifstream infile(Assets::DATADIR + "TestGrid1.txt");
    infile >> nodeSize;
    infile >> gridWidth;
    infile >> gridHeight;
    navGrid = new NavigationGrid("TestGrid1.txt");

    GridNode* nodes = new GridNode[gridWidth * gridHeight];
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x)
        {
            GridNode& n = nodes[(gridWidth * y) + x];
            char type = 0;
            infile >> type;
            n.type = type;
            n.position = Vector3((float)(x * nodeSize), 7, (float)(y * nodeSize));
            if (type == 120)scene.emplace_back(AddCubeToWorld(n.position, { (float)nodeSize / 2,(float)nodeSize / 2,(float)nodeSize / 2 }, 0));

        }
    }
    return;
}

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
    GameObject* floor = new GameObject("Floor");

    Vector3 floorSize = Vector3(200, 2, 200);
    AABBVolume* volume = new AABBVolume(floorSize);
    floor->SetBoundingVolume((CollisionVolume*)volume);
    floor->GetTransform()
        .SetScale(floorSize * 2)
        .SetPosition(position);

    floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, floorTex, basicShader));
    floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

    floor->GetPhysicsObject()->SetInverseMass(0);
    floor->GetPhysicsObject()->InitCubeInertia();

    world->AddGameObject(floor);

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

    world->AddGameObject(sphere);

    return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
    GameObject* cube = new GameObject();

    AABBVolume* volume = new AABBVolume(dimensions);
    cube->SetBoundingVolume((CollisionVolume*)volume);

    cube->GetTransform()
        .SetPosition(position)
        .SetScale(dimensions * 2);

    cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
    cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

    cube->GetPhysicsObject()->SetInverseMass(inverseMass);
    cube->GetPhysicsObject()->InitCubeInertia();

    world->AddGameObject(cube);

    return cube;
}

GameObject* TutorialGame::AddPlayer0ToWorld(const Vector3& position) {
    float meshSize = 1.0f;
    float inverseMass = 0.5f;

    character = new GameObject("Player");
    SphereVolume* volume = new SphereVolume(1.0f);

    character->SetBoundingVolume((CollisionVolume*)volume);

    character->GetTransform()
        .SetScale(Vector3(meshSize, meshSize, meshSize))
        .SetPosition(position);

    character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader));
    character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

    character->GetPhysicsObject()->SetInverseMass(inverseMass);
    character->GetPhysicsObject()->InitSphereInertia();

    world->AddGameObject(character);

    return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
    float meshSize = 3.0f;
    float inverseMass = 0.5f;

    GameObject* character = new GameObject("Enemy");

    AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
    character->SetBoundingVolume((CollisionVolume*)volume);

    character->GetTransform()
        .SetScale(Vector3(meshSize, meshSize, meshSize))
        .SetPosition(position);

    character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));// todo can change capsule
    character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

    character->GetPhysicsObject()->SetInverseMass(inverseMass);
    character->GetPhysicsObject()->InitSphereInertia();

    world->AddGameObject(character);

    return character;
}

TrainObject* TutorialGame::AddTrainToWorld(const Vector3& position) {
    TrainObject* train = new TrainObject(world, trainMesh, basicShader);

    SphereVolume* volume = new SphereVolume(0.5f);
    train->SetBoundingVolume((CollisionVolume*)volume);
    train->GetTransform()
        .SetScale(Vector3(2, 2, 2))
        .SetPosition(position);

    train->SetRenderObject(new RenderObject(&train->GetTransform(), trainMesh, nullptr, basicShader));
    train->SetPhysicsObject(new PhysicsObject(&train->GetTransform(), train->GetBoundingVolume()));

    train->GetPhysicsObject()->SetInverseMass(1.0f);
    train->GetPhysicsObject()->InitSphereInertia();

    world->AddGameObject(train);
    return train;
}

GameObject* TutorialGame::AddTestingLightToWorld(const Vector3& position, const Vector4& colour) {
    GameObject* cube = new GameObject();

    AABBVolume* volume = new AABBVolume(Vector3(0.5, 0.5, 0.5));
    cube->SetBoundingVolume((CollisionVolume*)volume);

    cube->GetTransform()
        .SetPosition(position)
        .SetScale(Vector3(1, 1, 1));

    cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, lightTex, bumpShader));
    cube->GetRenderObject()->SetBumpTexture(lightBumpTex);
    cube->GetRenderObject()->SetSpecTexture(lightSpecTex);
    cube->GetRenderObject()->SetColour(colour);
    cube->GetRenderObject()->SetEmissive(true);

    cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

    cube->GetPhysicsObject()->SetInverseMass(1);
    cube->GetPhysicsObject()->InitCubeInertia();

    world->AddGameObject(cube);

    return cube;
}

PlayerObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
    player = new PlayerObject();
    AABBVolume* volume = new AABBVolume(Vector3(1.5, 1.5, 1.5));
    player->SetBoundingVolume((CollisionVolume*)volume);

    player->GetTransform()
        .SetPosition(position)
        .SetScale(Vector3(3, 3, 3));

    player->SetRenderObject(new RenderObject(&player->GetTransform(), maleMesh, nullptr, skinningBumpShader, 3));
    player->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
    player->GetRenderObject()->SetMaterial(maleMaterial);
    player->GetRenderObject()->SetAnimationObject(maleAnimation);
    player->GetRenderObject()->SetTextures(maleTextures);
    player->GetRenderObject()->SetBumpTextures(maleBumpTextures);

    player->SetPlayerMeshes(meshes);
    player->SetPlayerTextures(textures);
    player->SetPlayerBumpTextures(bumpTextures);
    player->SetPlayerAnimations(animations);
    player->SetPlayerShaders(shaders);

    player->SetPhysicsObject(new PhysicsObject(&player->GetTransform(), player->GetBoundingVolume()));
    player->GetPhysicsObject()->SetInverseMass(1);
    player->GetPhysicsObject()->InitCubeInertia();

    world->AddGameObject(player);

    return player;
}

CollectableObject*TutorialGame::AddCollectableObjectToGround(int objectId)
{
    Vector3 GroundPos = findNearestGridCenter(PlayerFront());
    object->SetActive(false);
    groundObject = new CollectableObject(world);
    AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
    object->SetBoundingVolume((CollisionVolume*)volume);
    object->GetTransform().SetPosition(GroundPos);
    object->GetTransform().SetScale(Vector3(1, 1, 1));

    object->SetPhysicsObject(new PhysicsObject(&object->GetTransform(), object->GetBoundingVolume()));
    object->GetPhysicsObject()->SetInverseMass(1.0f);
    object->GetPhysicsObject()->InitCubeInertia();
    return object;

}
void TutorialGame::InitDefaultFloor() {
    AddFloorToWorld(Vector3(0, 0, 0));
}

void TutorialGame::InitGameExamples() {
    AddPlayer0ToWorld(Vector3(0, 5, 0));
    //AddEnemyToWorld(Vector3(5, 5, 0));
    trainObject = AddTrainToWorld(Vector3(10, 5, 0));
    trainObject->AddCarriage();
    trainObject->AddCarriage();
    AddTestingLightToWorld(Vector3(10, 20, 0), Vector4(1, 1, 1, 0.7));
    AddTestingLightToWorld(Vector3(30, 20, 40), Vector4(1, 0, 0, 0.7));
    AddTestingLightToWorld(Vector3(60, 20, 20), Vector4(0, 1, 0, 0.7));
    player = AddPlayerToWorld(Vector3(20, 5, 0));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
    for (int x = 0; x < numCols; ++x) {
        for (int z = 0; z < numRows; ++z) {
            Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
            AddSphereToWorld(position, radius, 1.0f);
        }
    }
    AddFloorToWorld(Vector3(0, -2, 0));
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

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
    for (int x = 1; x < numCols + 1; ++x) {
        for (int z = 1; z < numRows + 1; ++z) {
            Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
            AddCubeToWorld(position, cubeDims, 1.0f);
        }
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

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {
    StateGameObject* cube = new StateGameObject();

    Vector3 dimensions = Vector3(1, 1, 1);

    AABBVolume* volume = new AABBVolume(dimensions);
    cube->SetBoundingVolume((CollisionVolume*)volume);

    cube->GetTransform()
        .SetPosition(position)
        .SetScale(dimensions * 2);

    cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
    cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

    cube->GetPhysicsObject()->SetInverseMass(1.0f);
    cube->GetPhysicsObject()->InitCubeInertia();

    world->AddGameObject(cube);

    return cube;
}

CollectableObject* TutorialGame::CreateObject(int objectId)
{
    Vector3 PlayerPos = player->GetTransform().GetPosition();
    object = new CollectableObject(world);
    AABBVolume* volume = new AABBVolume(Vector3(1, 1, 1));
    object->SetBoundingVolume((CollisionVolume*)volume);
    object->GetTransform().SetPosition(PlayerPos);
    object->GetTransform().SetScale(Vector3(1, 1, 1));

    object->SetPhysicsObject(new PhysicsObject(&object->GetTransform(), object->GetBoundingVolume()));
    object->GetPhysicsObject()->SetInverseMass(1.0f);
    object->GetPhysicsObject()->InitCubeInertia();

    switch (objectId)
    {
    case 1:
        object->SetRenderObject(new RenderObject(&object->GetTransform(), sphereMesh, nullptr, basicShader));
        break;
    case 2:
        //�����Ʒ
        break;
    default:
        //�����Ʒ
        break;
    }
    world->AddGameObject(object);

    return object;

}

void TutorialGame::HoldObject()
{
    object->GetTransform().SetPosition(PlayerFront());
}

