#pragma once

#include "GameObject.h"
#include <vector>
using namespace NCL;
using namespace CSC8503;

namespace NCL::CSC8503 {
    class WaterObject : public GameObject {
    public:
        WaterObject() {
            typeID = 10000;
            name = "Water";
        };
        ~WaterObject() {};

        void OnCollisionBegin(GameObject* otherObject) override {};

        void OnCollisionEnd(GameObject* otherObject) override {};

        void Update(float dt) override;
    };
}