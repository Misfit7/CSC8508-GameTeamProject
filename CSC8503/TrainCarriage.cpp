

#include "TrainCarriage.h"


 TrainCarriage::TrainCarriage() {

}
 TrainCarriage::~TrainCarriage() {

}

void TrainCarriage::OnCollisionBegin(GameObject *otherObject) {

}

void TrainCarriage::OnCollisionEnd(GameObject *otherObject) {

}

void TrainCarriage::Update(float dt){
    auto it = path.begin();
    auto itt = it->first;
    Vector3 target = itt;
    Vector3 dir = (target - this->GetTransform().GetPosition());
    dir = Vector3(dir.x, 0, dir.z);
    GetPhysicsObject()->SetLinearVelocity(dir.Normalised() * 1000.0f * dt);
    if (this->GetTransform().GetPosition() == target)
        path.erase(it);
}