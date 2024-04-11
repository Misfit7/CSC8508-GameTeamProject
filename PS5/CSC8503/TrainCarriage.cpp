#include "TrainCarriage.h"
#include "TrainObject.h"
#include "TutorialGame.h"
#include "NetworkedGame.h"

using namespace NCL::CSC8503;

TrainCarriage::TrainCarriage() {

}

TrainCarriage::~TrainCarriage() {

}

void TrainCarriage::OnCollisionBegin(GameObject* otherObject) {

}

void TrainCarriage::OnCollisionEnd(GameObject* otherObject) {

}

void TrainCarriage::UpdateOrientation() {
    Quaternion rotation;
    if (direction.x > 0) rotation = Quaternion::EulerAnglesToQuaternion(0, -90, 0);
    else if (direction.x < 0) rotation = Quaternion::EulerAnglesToQuaternion(0, 90, 0);
    else if (direction.z > 0) rotation = Quaternion::EulerAnglesToQuaternion(0, 180, 0);
    else if (direction.z < 0) rotation = Quaternion::EulerAnglesToQuaternion(0, 0, 0);
    transform.SetOrientation(rotation);
}

int TrainCarriage::GetDirection() {
    if (direction.x > 0) return 1;
    else if (direction.x < 0) return 2;
    else if (direction.z > 0) return 3;
    else if (direction.z < 0) return 4;
}

void TrainCarriage::Update(float dt) {
    if (path.size() == 0) return;
    Vector3 target = path[0];
    direction = (target - this->GetTransform().GetPosition());
    direction = Vector3(direction.x, 0, direction.z);
    GetPhysicsObject()->SetLinearVelocity(Maths::Vector::Normalise(direction) * TutorialGame::GetGame()->GetTrain()->GetForce() * dt);
    UpdateOrientation();

    float mm = Maths::Vector::Length(this->GetTransform().GetPosition() - target);
    if (mm < 0.5) {
        if (GetDirection() < 3) transform.SetPosition(Vector3(target.x, transform.GetPosition().y, transform.GetPosition().z));
        else transform.SetPosition(Vector3(transform.GetPosition().x, transform.GetPosition().y, target.z));
        physicsObject->SetLinearVelocity(Vector3());
        path.erase(path.begin());
    }
}

void TrainCarriage::AddPath(Vector3 p) {
    this->path.push_back(p);
}

void MaterialCarriage::Update(float dt) {
    if (path.size() != 0) {
        Vector3 target = path[0];
        direction = (target - this->GetTransform().GetPosition());
        direction = Vector3(direction.x, 0, direction.z);
        //std::cout << "Dir: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
        GetPhysicsObject()->SetLinearVelocity(Maths::Vector::Normalise(direction) * TutorialGame::GetGame()->GetTrain()->GetForce() * dt);
        UpdateOrientation();

        float mm = Maths::Vector::Length(this->GetTransform().GetPosition() - target);
        if (mm < 0.5) {
            if (GetDirection() < 3) transform.SetPosition(Vector3(target.x, transform.GetPosition().y, transform.GetPosition().z));
            else transform.SetPosition(Vector3(transform.GetPosition().x, transform.GetPosition().y, target.z));
            physicsObject->SetLinearVelocity(Vector3());
            path.erase(path.begin());
        }
    }

    if (produceCarriage->Finished()) {
        if (!planks.empty() && !stones.empty()) {
            ready = true;
        }
        else {
            ready = false;
        }
    }

    //std::cout << "Plank: " << planks.size() << std::endl;
    //std::cout << "Stone: " << stones.size() << std::endl;
}

void MaterialCarriage::UpdateMaterial() {
    world->RemoveGameObject(planks[0]);
    planks.erase(planks.begin());
    if (!planks.empty()) {
        for (size_t i = 0; i < planks.size(); ++i) {
            planks[i]->SetHeight(transform.GetPosition().y + 4 + i);
        }
    }
    world->RemoveGameObject(stones[0]);
    stones.erase(stones.begin());
    if (!stones.empty()) {
        for (size_t i = 0; i < stones.size(); ++i) {
            stones[i]->SetHeight(transform.GetPosition().y + 4 + i);
        }
    }
}

void ProduceCarriage::OnCollisionBegin(GameObject* otherObject) {
    if (otherObject->GetTypeID() == 1) {
        if (!rails.empty()) {
            if (otherObject->GetSlot() == 0) {
                int num = 0;
                num = rails.size() > 3 ? 3 : rails.size();
                for (int i = 0; i < num; ++i) {
                    if (rails[0]) {
                        rails[0]->SetPlayer((PlayerObject*)otherObject);
                        rails[0]->SetPutDown(false);
                        rails[0]->SetInCarriage(false);
                        rails[0]->SetNum(otherObject->GetSlotNum() + 1);
                        otherObject->SetSlot(7);
                        otherObject->SetSlotNum(otherObject->GetSlotNum() + 1);
                        rails.erase(rails.begin());
                        for (size_t i = 0; i < rails.size(); ++i) {
                            rails[i]->SetHeight(transform.GetPosition().y + 4 + i);
                        }
                    }
                }
            }
            else if (otherObject->GetSlot() == 7 && otherObject->GetSlotNum() < 3) {
                for (int i = 0; i < 3 - otherObject->GetSlotNum(); ++i) {
                    if (rails[0]) {
                        rails[0]->SetPlayer((PlayerObject*)otherObject);
                        rails[0]->SetPutDown(false);
                        rails[0]->SetInCarriage(false);
                        rails[0]->SetNum(otherObject->GetSlotNum() + 1);
                        otherObject->SetSlotNum(otherObject->GetSlotNum() + 1);
                        rails.erase(rails.begin());
                        for (size_t i = 0; i < rails.size(); ++i) {
                            rails[i]->SetHeight(transform.GetPosition().y + 4 + i);
                        }
                    }
                }
            }
        }
    }
}

void ProduceCarriage::Update(float dt) {
    if (path.size() != 0) {
        Vector3 target = path[0];
        direction = (target - this->GetTransform().GetPosition());
        direction = Vector3(direction.x, 0, direction.z);
        //std::cout << "Dir: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
        GetPhysicsObject()->SetLinearVelocity(Maths::Vector::Normalise(direction) * TutorialGame::GetGame()->GetTrain()->GetForce() * dt);
        UpdateOrientation();

        float mm = Maths::Vector::Length(this->GetTransform().GetPosition() - target);
        if (mm < 0.5) {
            if (GetDirection() < 3) transform.SetPosition(Vector3(target.x, transform.GetPosition().y, transform.GetPosition().z));
            else transform.SetPosition(Vector3(transform.GetPosition().x, transform.GetPosition().y, target.z));
            physicsObject->SetLinearVelocity(Vector3());
            path.erase(path.begin());
        }
    }

    if (finish && materialCarriage->IsReady()) {
        finish = false;
        counter = 3.0f;
        materialCarriage->UpdateMaterial();
    }
    if (!finish) {
        counter -= dt;
        renderObject->SetColour(Vector4(1, 1, counter / 3, 1));
    }
    if (counter <= 0 && !finish) {
        if (rails.size() < 10) {
            finish = true;
            RailObject* rail = TutorialGame::GetGame()->AddRailToWorld(transform.GetPosition());
            rail->SetCarriage(this);
            rail->SetHeight(transform.GetPosition().y + 4 + rails.size());
            rails.push_back(rail);
        }
    }
}

void WaterCarriage::Update(float dt) {
    if (path.size() != 0) {
        Vector3 target = path[0];
        direction = (target - this->GetTransform().GetPosition());
        direction = Vector3(direction.x, 0, direction.z);
        //std::cout << "Dir: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
        GetPhysicsObject()->SetLinearVelocity(Maths::Vector::Normalise(direction) * TutorialGame::GetGame()->GetTrain()->GetForce() * dt);
        UpdateOrientation();

        float mm = Maths::Vector::Length(this->GetTransform().GetPosition() - target);
        if (mm < 0.5) {
            if (GetDirection() < 3) transform.SetPosition(Vector3(target.x, transform.GetPosition().y, transform.GetPosition().z));
            else transform.SetPosition(Vector3(transform.GetPosition().x, transform.GetPosition().y, target.z));
            physicsObject->SetLinearVelocity(Vector3());
            path.erase(path.begin());
        }
    }

    float speed = 1.0f;
    float1 = water;
    if (water > 0.0f) water -= dt * speed;
    float color = water / 100.f;
    renderObject->SetColour(Vector4(1 - color, 0, color, 1));

    if (water <= 0) {
        train->SetOnFire(true);
    }
}