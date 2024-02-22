#pragma once

#include"PhysicsSystem.h"
#include"PhysicsObject.h"
#include "StateMachine.h"
#include "NavigationGrid.h"
#include "StateTransition.h"
#include "State.h"
#include "StateMachine.h"
#include "PositionConstraint.h"
#include "NavigationGrid.h"
#include "GameObject.h"
#include <vector>
#include "RenderObject.h"
#include "TrainCarriage.h"
using namespace NCL;
using namespace CSC8503;

class TreeObject : public GameObject {

public:
    TreeObject() {};
    ~TreeObject() {};

    void OnCollisionBegin(GameObject* otherObject) override {};

    void OnCollisionEnd(GameObject* otherObject) override {};

    void Setscale(float a) {
        scale = a;
    }
    float Getscale() {
        return scale;
    }
    // void Update(float dt) override;
    bool moving = false;
    float scale;
};