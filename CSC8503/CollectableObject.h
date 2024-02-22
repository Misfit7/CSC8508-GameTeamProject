#pragma once

#include "GameObject.h"
#include "GameWorld.h"
#include "PlayerObject.h"
#include <vector>

namespace NCL::CSC8503 {
    struct GridCenter {
        int x;
        int y;
    };

    class CollectableObject : public GameObject {
    public:
        CollectableObject(GameWorld* world, std::string name = "") : GameObject(name) {
            this->name = name;
            this->world = world;
            putDown = true;
            player = nullptr;
            triggerDelete = true;
        };
        CollectableObject() {
            putDown = true;
            player = nullptr;
            triggerDelete = true;
        };
        ~CollectableObject() {};

        void Update(float dt) override;

        void OnCollisionBegin(GameObject* otherObject) override;

        void SetPlayer(PlayerObject* player) {
            this->player = player;
        }

        Vector3 findNearestGridCenter(const Vector3& position);
        Vector3 PlayerFront();

    protected:
        bool putDown;
        PlayerObject* player;
        GameWorld* world;

        const int GRID_SIZE_X = 10;
        const int GRID_SIZE_Y = 10;
        const float GRID_CENTER_X = 5.0f;
        const float  GRID_CENTER_Y = 5.0f;

        float distance(const Vector3& v1, const Vector3& v2);
        double euclideanDistance(float x1, float y1, float x2, float y2);
    };
}