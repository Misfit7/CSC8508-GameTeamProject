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
#include "TrainCarriage.h"

namespace NCL::CSC8503 {
    class TrainCarriage : public GameObject {
    public:
        TrainCarriage();

        ~TrainCarriage();

        void OnCollisionBegin(GameObject* otherObject) override;

        void OnCollisionEnd(GameObject* otherObject) override;

        void Update(float dt);

        void UpdateOrientation(Vector3 direction);

        std::vector< std::pair<Vector3, int> > path;
    };;
}