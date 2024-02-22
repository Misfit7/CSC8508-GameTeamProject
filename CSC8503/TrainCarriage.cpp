#include "TrainCarriage.h"
#include "TrainObject.h"

using namespace NCL::CSC8503;

TrainCarriage::TrainCarriage() {

}

TrainCarriage::~TrainCarriage() {

}

void TrainCarriage::OnCollisionBegin(GameObject *otherObject) {

}

void TrainCarriage::OnCollisionEnd(GameObject *otherObject) {

}

Quaternion RotateBetweenVectors2(const Vector3 &from, const Vector3 &to) {
    Vector3 source = from.Normalised(); // 首先，计算两个向量的标准化版本
    Vector3 target = to.Normalised();
    float dotProduct = Vector3::Dot(source, target);// 计算两个向量之间的点积
    if (dotProduct < -0.999f) { // 如果点积接近于-1，表示两个向量相反
        Vector3 axis = Vector3::Cross(Vector3(1.0f, 0.0f, 0.0f), source);  // 选择任意垂直于 source 向量的向量作为旋转轴
        if (axis.LengthSquared() < 0.01f) {
            axis = Vector3::Cross(Vector3(0.0f, 1.0f, 0.0f), source);
        }
        axis.Normalise();
        return Quaternion(axis, 0.0f);  // 返回一个绕选择的轴旋转 180 度的四元数
    }
    Vector3 rotationAxis = Vector3::Cross(source, target);// 计算旋转轴
    rotationAxis.Normalise();
    float rotationAngle = std::acos(dotProduct);// 计算旋转角度
    return Quaternion(rotationAxis, rotationAngle);// 构建旋转四元数
}

void TrainCarriage::UpdateOrientation(Vector3 direction) {
    Quaternion rotation;
    if (direction.x > 0) rotation = Quaternion::EulerAnglesToQuaternion(0, -90, 0);
    else if (direction.x < 0) rotation = Quaternion::EulerAnglesToQuaternion(0, 90, 0);
    else if (direction.z > 0) rotation = Quaternion::EulerAnglesToQuaternion(0, 180, 0);
    else if (direction.z < 0) rotation = Quaternion::EulerAnglesToQuaternion(0, 0, 0);
    transform.SetOrientation(rotation);
}

void TrainCarriage::Update(float dt) {
    if (path.size() == 0) return;
    auto it = path.begin();
    auto itt = it->first;
    int flag = it->second;
    if (flag <= 1) {
        Vector3 newDirection(0.0f, 0.0f, 1.0f);
        Vector3 currentDirection = this->GetTransform().GetMatrix() * Vector3(0.0f, 0.0f, 1.0f);
        Quaternion rotation = RotateBetweenVectors2(currentDirection, newDirection);
        this->GetTransform().SetOrientation(rotation);

    } else {
        Vector3 newDirection(1.0f, 0.0f, 0.0f);
        Vector3 currentDirection = this->GetTransform().GetMatrix() * Vector3(0.0f, 0.0f, 1.0f);
        Quaternion rotation = RotateBetweenVectors2(currentDirection, newDirection);
        this->GetTransform().SetOrientation(rotation);

    }
    Vector3 target = itt;
    Vector3 dir = (target - this->GetTransform().GetPosition());
    dir = Vector3(dir.x, 0, dir.z);
    GetPhysicsObject()->SetLinearVelocity(dir.Normalised() * 1000.0f * dt);
    float mm = (this->GetTransform().GetPosition() - target).Length();
    if (mm < 0.5) {
        path.erase(it);
    }
}