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

        const std::string& GetName() const {
            return name;
        }

        virtual void OnCollisionBegin(GameObject* otherObject) {
            //std::cout << "OnCollisionBegin event occured!\n";
        }

        virtual void OnCollisionEnd(GameObject* otherObject) {
            //std::cout << "OnCollisionEnd event occured!\n";
        }

        bool GetBroadphaseAABB(Vector3& outsize) const;

        void UpdateBroadphaseAABB();

        void SetWorldID(int newID) {
            worldID = newID;
        }

        int		GetWorldID() const {
            return worldID;
        }
        bool triggerDelete;

    protected:
        Transform			transform;

        CollisionVolume* boundingVolume;
        float boundingRadius = 1.0f;
        PhysicsObject* physicsObject;
        RenderObject* renderObject;
        NetworkObject* networkObject;

        bool		isActive;
        int			worldID;
        std::string	name;

        Vector3 broadphaseAABB;
    };
}

