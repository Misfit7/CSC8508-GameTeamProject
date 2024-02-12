
#pragma once

#include"PhysicsSystem.h"
#include"PhysicsObject.h"
#include "StateMachine.h"
#include "NavigationGrid.h"
#include "StateTransition.h"
#include "State.h"
#include "StateMachine.h"
#include "NavigationGrid.h"
#include "GameObject.h"
#include <vector>
#include "RenderObject.h"
using namespace NCL;
using namespace CSC8503;

class TrainObject : public GameObject {

public:
    TrainObject();

    ~TrainObject();

    void OnCollisionBegin(GameObject *otherObject) override;

    void OnCollisionEnd(GameObject *otherObject) override;

    void Update(float dt);

    void UpdatePath(  std::vector<Vector3> p);
    void  MoveToNextPoiont(float dt);
public:
    std::vector<Vector3> path;

};


