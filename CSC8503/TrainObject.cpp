

#include "TrainObject.h"
#include "TutorialGame.h"

TrainObject::TrainObject() {
    path.push_back({Vector3(10, 5, 60), 4});

   // path.push_back({Vector3(60, 0, 60), 1});
}

TrainObject::~TrainObject() {

}

TrainObject::TrainObject(GameWorld *w, OBJMesh *mesh, ShaderGroup *shader) {

    path.push_back({Vector3(10, 5, 60), 4});
    path.push_back({Vector3(60, 0, 60), 1});
    world = w;
    trainMesh = mesh;
    basicShader = shader;
    trainCarriage = new TrainCarriage[20];
    trainIndex = 0;
}

void TrainObject::OnCollisionBegin(GameObject *otherObject) {

}

void TrainObject::OnCollisionEnd(GameObject *otherObject) {

}

void TrainObject::Update(float dt) {
    auto it = path.begin();
    auto itt = it->first;
    int flag = it->second;
    if(flag<=1){
//        float angle = 90.0f; // 旋转角度，单位为度
//        Vector3 rotationAxis(0, 1, 0); // 旋转轴，例如绕Y轴旋转
//        GetTransform().SetOrientation(Quaternion(rotationAxis,angle));
    }
    else{
//        float angle = 90.0f; // 旋转角度，单位为度
//        Vector3 rotationAxis(0, 1, 0); // 旋转轴，例如绕Y轴旋转
//        GetTransform().SetOrientation(Quaternion(rotationAxis,angle));
    }
    Vector3 target = itt;
    Vector3 dir = (target - this->GetTransform().GetPosition());
    dir = Vector3(dir.x, 0, dir.z);
    GetPhysicsObject()->SetLinearVelocity(dir.Normalised() * 1000.0f * dt);
    if (this->GetTransform().GetPosition() == target)
        path.erase(it);
    for(int i = 1; i<=trainIndex ;i ++ )
        trainCarriage[i].Update(dt);
}

void TrainObject::UpdatePath(std::vector<std::pair<Vector3, int>> p) {
    path = p;
}


void TrainObject::AddCarriage() {
    Vector3 nowPos;
    if (trainIndex == 0)
        nowPos = GetTransform().GetPosition();
    else
        nowPos = trainCarriage[trainIndex].GetTransform().GetPosition();

    Vector3 nextPos;


    if (path.front().second <= 1) { //车头方向为上下 ，添加的车厢竖值放置
        nextPos = nowPos;
        nextPos.x-=3;
    } else {
        nextPos = nowPos;
        nextPos.z-=3;

    }
    std::cout<<nowPos<<std::endl;

    std::cout<<nextPos<<std::endl;

    TrainCarriage *carriage = new TrainCarriage;
    carriage->path = path;
    SphereVolume *volume = new SphereVolume(0.5f);
    carriage->SetBoundingVolume((CollisionVolume *) volume);
    carriage->GetTransform()
            .SetScale(Vector3(2, 2, 2))
            .SetPosition(nextPos);

    carriage->SetRenderObject(new RenderObject(&carriage->GetTransform(), trainMesh, nullptr, basicShader));
    carriage->SetPhysicsObject(new PhysicsObject(&carriage->GetTransform(), carriage->GetBoundingVolume()));

    carriage->GetPhysicsObject()->SetInverseMass(1.0);
    carriage->GetPhysicsObject()->InitSphereInertia();

    trainCarriage[++trainIndex] = *carriage;

    world->AddGameObject(carriage);

//    if(trainIndex==1){
//        AddConstraint(this,&trainCarriage[trainIndex]);
//    }
//    else{
//        AddConstraint(&trainCarriage[trainIndex-1],&trainCarriage[trainIndex]);
//    }

}
void TrainObject::AddConstraint(GameObject *a,GameObject *b){
    float maxDistance = 10.0f;
    PositionConstraint* constraint = new PositionConstraint(a, b, maxDistance);
    world->AddConstraint(constraint);
}
//void TutorialGame::BridgeConstraintTest() {
//    Vector3 cubeSize = Vector3(8, 8, 8);
//    float invCubeMass = 5;  // how heavy the middle pieces are
//    int numLinks = 10;
//    float maxDistance = 30;  // constraint distance
//    float cubeDistance = 20;  // distance between links
//
//    Vector3 startPos = Vector3(5, 100, 5);
//
//    GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
//    GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);
//
//    GameObject* previous = start;
//
//    for (int i = 0; i < numLinks; ++i) {
//        GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
//        PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
//        world->AddConstraint(constraint);
//        previous = block;
//    }
//    PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
//    world->AddConstraint(constraint);
//}

