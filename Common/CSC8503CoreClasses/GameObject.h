#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "CapsuleVolume.h"

using std::vector;

namespace NCL::CSC8503 {
    class NetworkObject;
    class RenderObject;
    class PhysicsObject;

    struct GridCenter {
        int x;
        int z;
    };

    class GameObject {
    public:
        GameObject(const std::string& name = "");
        ~GameObject();

        void SetBoundingVolume(CollisionVolume* vol) {
            boundingVolume = vol;
            Vector3 a;
            switch (boundingVolume->type)
            {
            case VolumeType::AABB:
                a = ((AABBVolume&)*vol).GetHalfDimensions();
                SetBoundingRadius(std::max(a.x, std::max(a.y, a.z)));
                break;
            case VolumeType::OBB:
                a = ((OBBVolume&)*vol).GetHalfDimensions();
                SetBoundingRadius(std::max(a.x, std::max(a.y, a.z)));
                break;
            case VolumeType::Sphere:
                SetBoundingRadius(((SphereVolume&)*vol).GetRadius());
                break;
            case VolumeType::Capsule:
                SetBoundingRadius(((CapsuleVolume&)*vol).GetHalfHeight());
                break;
            default:
                a = ((AABBVolume&)*vol).GetHalfDimensions();
                SetBoundingRadius(std::max(a.x, std::max(a.y, a.z)));
                break;
            }
        }

        const CollisionVolume* GetBoundingVolume() const {
            return boundingVolume;
        }

        float GetBoundingRadius() const { return boundingRadius; }
        void SetBoundingRadius(float f) { boundingRadius = f; }

        bool IsActive() const {
            return isActive;
        }
        void SetActive(bool active) {
            isActive = active;
        }

        bool IsSpawned() const {
            return spawned;
        }
        void SetSpawned(bool s) {
            spawned = s;
        }

        bool UpdateInClient() const {
            return updateInClient;
        }
        void SetUpdateInClient(bool u) {
            updateInClient = u;
        }

        Transform& GetTransform() {
            return transform;
        }

        RenderObject* GetRenderObject() const {
            return renderObject;
        }

        PhysicsObject* GetPhysicsObject() const {
            return physicsObject;
        }

        NetworkObject* GetNetworkObject() const {
            return networkObject;
        }

        void SetRenderObject(RenderObject* newObject) {
            renderObject = newObject;
        }

        void SetPhysicsObject(PhysicsObject* newObject) {
            physicsObject = newObject;
        }

        void SetNetworkObject(NetworkObject* newObject) {
            networkObject = newObject;
        }

        void SetName(std::string n) {
            name = n;
        }
        const std::string& GetName() const {
            return name;
        }

        virtual void OnCollisionBegin(GameObject* otherObject) {
            //std::cout << "OnCollisionBegin event occured!\n";
        }

        virtual void OnCollisionEnd(GameObject* otherObject) {
            //std::cout << "OnCollisionEnd event occured!\n";
        }

        virtual void Update(float dt){}

        bool GetBroadphaseAABB(Vector3& outsize) const;

        void UpdateBroadphaseAABB();

        void SetWorldID(int newID) {
            worldID = newID;
        }
        int		GetWorldID() const {
            return worldID;
        }

        void SetTypeID(int newID) {
            typeID = newID;
        }
        int		GetTypeID() const {
            return typeID;
        }

        void SetSlot(int s) {
            slot = s;
        }
        int GetSlot() const {
            return slot;
        }

        void SetSlotNum(int n) {
            slotNum = n;
        }
        int GetSlotNum() const {
            return slotNum;
        }

        void SetFlag1(int f) {
            flag1 = f;
        }
        bool GetFlag1() const {
            return flag1;
        }
        void SetFlag2(int f) {
            flag2 = f;
        }
        bool GetFlag2() const {
            return flag2;
        }
        void SetFlag3(int f) {
            flag3 = f;
        }
        bool GetFlag3() const {
            return flag3;
        }
        void SetFlag4(int f) {
            flag4 = f;
        }
        bool GetFlag4() const {
            return flag4;
        }
        void SetFlag5(int f) {
            flag5 = f;
        }
        bool GetFlag5() const {
            return flag5;
        }

        void SetFloat1(float f) {
            float1 = f;
        }
        float GetFloat1() const {
            return float1;
        }
        void SetFloat2(float f) {
            float2 = f;
        }
        float GetFloat2() const {
            return float2;
        }
        void SetFloat3(float f) {
            float3 = f;
        }
        float GetFloat3() const {
            return float3;
        }

        Vector3 GetPosition1() const {
            return position1;
        }
        void SetPosition1(Vector3 p) {
            position1 = p;
        }

        void SetButton(int index, bool state) {
            buttonStates[index] = state;
        }
        bool GetButton(int index) const {
            return buttonStates[index];
        }

        bool ShowDebug() const {
            return showDebug;
        }

        double EuclideanDistance(float x1, float y1, float x2, float y2);
        Vector3 FindNearestGridCenter(const Vector3& position);
        Vector3 FindGrid(const Vector3& position);

    protected:
        Transform			transform;

        CollisionVolume* boundingVolume;
        float boundingRadius = 1.0f;
        PhysicsObject* physicsObject;
        RenderObject* renderObject;
        NetworkObject* networkObject;

        bool		isActive;
        bool        triggerDelete;
        bool        spawned = true;
        bool        updateInClient = false;
        int			worldID;
        int         typeID;
        int         slot = 0;
        int         slotNum = 0;
        std::string	name;

        int GRID_SIZE_X = 32;
        int GRID_SIZE_Z = 20;
        float GRID_CENTER_X = 5.0f;
        float  GRID_CENTER_Z = 5.0f;
        int GridSize=10;

        bool flag1 = false;
        bool flag2 = false;
        bool flag3 = false;
        bool flag4 = false;
        bool flag5 = false;

        bool showDebug = false;

        float float1 = 0.0f;
        float float2 = 0.0f;
        float float3 = 0.0f;

        Vector3 position1;

        Vector3 broadphaseAABB;

        bool buttonStates[9];
    };
}

