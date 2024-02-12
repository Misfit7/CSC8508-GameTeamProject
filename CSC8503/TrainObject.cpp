

#include "TrainObject.h"

TrainObject::TrainObject() {
    path.push_back(Vector3(600, 0, 0));

}

TrainObject::~TrainObject() {

}

void TrainObject::OnCollisionBegin(GameObject *otherObject) {

}

void TrainObject::OnCollisionEnd(GameObject *otherObject) {

}

void TrainObject::Update(float dt) {
    auto it = path.begin();
    Vector3 target = *it;
    Vector3 dir = (target - this->GetTransform().GetPosition());
    dir = Vector3(dir.x, 0, dir.z);
    GetPhysicsObject()->SetLinearVelocity(dir.Normalised() * 1000.0f * dt);
    if (this->GetTransform().GetPosition() == target)
        path.erase(it);
}

void TrainObject::UpdatePath(std::vector<Vector3> p) {
    path = p;
}

