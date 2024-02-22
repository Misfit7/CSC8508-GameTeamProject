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

namespace NCL::CSC8503 {
    class PlayerObject : public GameObject {

    public:
        PlayerObject(std::string name) {
            this->name = name;
        };
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
        void SetPlayerBumpTextures(vector<vector<GLuint>> t) {
            bumpTextures = t;
        }
        void SetPlayerAnimations(vector<AnimationObject*> a) {
            animations = a;
        }
        void SetPlayerShaders(vector<ShaderGroup*> s) {
            shaders = s;
        }

    protected:
        int index;
        vector<Mesh*> meshes;
        vector<vector<GLuint>> textures;
        vector<vector<GLuint>> bumpTextures;
        vector<AnimationObject*> animations;
        vector<ShaderGroup*> shaders;
    };
}