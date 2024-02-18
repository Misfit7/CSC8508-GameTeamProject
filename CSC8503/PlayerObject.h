
#pragma once

#include "PhysicsSystem.h"
#include "PhysicsObject.h"
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
#include "AnimationObject.h"
#include "TrainCarriage.h"
using namespace NCL;
using namespace CSC8503;

class PlayerObject : public GameObject {

public:
    PlayerObject() {};
    ~PlayerObject() {};

    void OnCollisionBegin(GameObject* otherObject) override {};

    void OnCollisionEnd(GameObject* otherObject) override {};

    void Update(float dt) override;

    void SetPlayerMeshes(vector<Mesh*> m) {
        meshes = m;
    }
    void SetPlayerTextures(vector<vector<GLuint>> t) {
        textures = t;
    }
    void SetPlayerAnimations(vector<AnimationObject*> a) {
        animations = a;
    }

protected:
    int index;
    vector<Mesh*> meshes;
    vector<vector<GLuint>> textures;
    vector<AnimationObject*> animations;
};


