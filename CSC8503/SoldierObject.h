
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

class SoldierObject : public GameObject {

public:
    SoldierObject() {};
    ~SoldierObject() {};

    void OnCollisionBegin(GameObject* otherObject) override {};

    void OnCollisionEnd(GameObject* otherObject) override {};

    void Update(float dt) override;
};


