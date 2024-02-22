#include "CollectableObject.h"
#include "Window.h"

using namespace NCL::CSC8503;

void CollectableObject::Update(float dt) {
    if (putDown) {
        physicsObject->AddTorque(Vector3(10, 10, 0));
    }
    else {
        Vector3 playerPosition = player->GetTransform().GetPosition();
        transform.SetPosition(Vector3(playerPosition.x, playerPosition.y + 5, playerPosition.z));
        transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, 0));
    }
    if (Window::GetKeyboard()->KeyPressed(KeyCodes::R)) {
        if (!putDown) {
            putDown = true;
            Vector3 position = transform.GetPosition();
            transform.SetPosition(Vector3(position.x + 10, 5, position.z));
        }
    }
}

void CollectableObject::OnCollisionBegin(GameObject* otherObject)  {
    if (putDown && otherObject->GetName() == "Player") {
       // if (!player->GetPowerup())
            putDown = false;
    }
}

float CollectableObject::distance(const Vector3& v1, const Vector3& v2) {
    float dx = v1.x - v2.x;
    float dy = v1.y - v2.y;
    float dz = v1.z - v2.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

double CollectableObject::euclideanDistance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

Vector3 CollectableObject::findNearestGridCenter(const Vector3& position) {
    float inputX = position.x;
    float inputY = position.y;
    GridCenter nearestGridCenter;
    Vector3 _nearestGridCenter;
    double minDistance = 1000;
    for (int x = 0; x < GRID_SIZE_X; ++x) {
        for (int y = 0; y < GRID_SIZE_Y; ++y) {
            double distance = euclideanDistance(inputX, inputY, x * 10 + GRID_CENTER_X, y * 10 + GRID_CENTER_Y);
            if (distance < minDistance) {
                minDistance = distance;
                nearestGridCenter.x = x * 10 + GRID_CENTER_X;
                nearestGridCenter.y = y * 10 + GRID_CENTER_Y;
            }
        }
    }
    _nearestGridCenter.x = nearestGridCenter.x;
    _nearestGridCenter.y = nearestGridCenter.y;
    _nearestGridCenter.z = 5;
    return _nearestGridCenter;
}

Vector3 CollectableObject::PlayerFront()
{
    Vector3 playerPos = player->GetTransform().GetPosition();
    Quaternion facingDir = player->GetTransform().GetOrientation();
    Vector3 ObjectOffset(0, 2, -5);
    Vector3 ObjectPos = facingDir * ObjectOffset;
    ObjectOffset = facingDir * ObjectOffset;
    Vector3 finalObjectPos = ObjectPos + playerPos;
    return finalObjectPos;
}