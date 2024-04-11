#include "GameObject.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

using namespace NCL::CSC8503;

GameObject::GameObject(const std::string& objectName) {
    name = objectName;
    worldID = -1;
    isActive = true;
    boundingVolume = nullptr;
    physicsObject = nullptr;
    renderObject = nullptr;
    networkObject = nullptr;
}

GameObject::~GameObject() {
    delete boundingVolume;
    delete physicsObject;
    delete renderObject;
    delete networkObject;
}

bool GameObject::GetBroadphaseAABB(Vector3& outSize) const {
    if (!boundingVolume) {
        return false;
    }
    outSize = broadphaseAABB;
    return true;
}

void GameObject::UpdateBroadphaseAABB() {
    if (!boundingVolume) {
        return;
    }
    if (boundingVolume->type == VolumeType::AABB) {
        broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
    }
    else if (boundingVolume->type == VolumeType::Sphere) {
        float r = ((SphereVolume&)*boundingVolume).GetRadius();
        broadphaseAABB = Vector3(r, r, r);
    }
    else if (boundingVolume->type == VolumeType::OBB) {
        Matrix3 mat = Matrix3(transform.GetOrientation());
        mat = mat.Absolute();
        Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
        broadphaseAABB = mat * halfSizes;
    }
}

double GameObject::EuclideanDistance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

Vector3 GameObject::FindNearestGridCenter(const Vector3& position) {
    /*float inputX = position.x;
    float inputZ = position.z;
    GridCenter nearestGridCenter;
    Vector3 _nearestGridCenter;
    double minDistance = 1000;
    for (int x = 0; x < GRID_SIZE_X; ++x) {
        for (int z = 0; z < GRID_SIZE_Z; ++z) {
            double distance = EuclideanDistance(inputX, inputZ, x * 10 + GRID_CENTER_X, z * 10 + GRID_CENTER_Z);
            if (distance < minDistance) {
                minDistance = distance;
                nearestGridCenter.x = x * 10 + GRID_CENTER_X;
                nearestGridCenter.z = z * 10 + GRID_CENTER_Z;
            }
        }
    }
    _nearestGridCenter.x = nearestGridCenter.x;
    _nearestGridCenter.z = nearestGridCenter.z;
    _nearestGridCenter.y = position.y;
    return _nearestGridCenter;*/
    return position;
}
Vector3 GameObject::FindGrid(const Vector3& position) {
    Vector3 p;
    p.x = (float)(((int)((int)position.x / (int)GridSize)) * (int)GridSize);
    p.z = (float)(((int)((int)position.z / (int)GridSize)) * (int)GridSize);
    return Vector3(p.x, position.y, p.z);
}