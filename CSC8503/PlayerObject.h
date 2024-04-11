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
#include "NetworkObject.h"
#include "TrainCarriage.h"

namespace NCL::CSC8503 {
    class BucketObject;
    class PlayerObject : public GameObject {

    public:
        PlayerObject(std::string name) {
            this->name = name;
            typeID = 1;
        };
        ~PlayerObject() {};

        void OnCollisionBegin(GameObject* otherObject) override;

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

        int GetNum() const {
            return num;
        }
        void SetNum(int n) {
            num = n;
        }

        void SetFace(Vector3 a) {
            face = a;
        }
        void SetSpeed(float a) {
            speed = a;
        }
        float GetSpeed() const {
            return speed;
        }
        Vector3 GetFace() const {
            return face;
        }

        bool IsPlacing1() const {
            return placing1;
        }
        void SetPlacing1(bool p) {
            placing1 = p;
        }
        bool IsPlacing2() const {
            return placing2;
        }
        void SetPlacing2(bool p) {
            placing2 = p;
        }
        bool IsBuilding() const {
            return building;
        }
        void SetBuilding(bool b) {
            building = b;
        }

        bool RobotCut() const {
            return robotCut;
        }
        void SetRobotCut(bool c) {
            robotCut = c;
        }
        bool RobotDig() const {
            return robotDig;
        }
        void SetRobotDig(bool d) {
            robotDig = d;
        }

        MaterialCarriage* GetCarriage() const {
            return carriage;
        }
        void SetCarriage(MaterialCarriage* c) {
            carriage = c;
        }
        Vector3 GetBridgePosition() {
            return bridgePosition;
        }

        bool CanPlaceRail();
        void UpdateAnimation(float dt);
        void PlayerMovement(float dt);
        void UpdateFace();

    protected:
        void CutTree();
        void DigRock();
        void ScoopWater();
        void UseWater();
        void BuildBridge();
        void LoadMaterial();
        void UseRobot();
        void RunFast(float dt);

        bool doing = false;
        bool cutting = false;
        bool digging = false;
        bool placing1 = false;
        bool placing2 = false;
        bool building = false;
        bool running = false;

        float coolDown = 2.0f;
        float runPower = 0.0f;
        float range = 10.0f;

        bool robotCut = false;
        bool robotDig = false;

        int num;
        int index;
        Vector3 face;
        Vector3 bridgePosition;
        float speed = 50.0f;
        vector<Mesh*> meshes;
        vector<vector<GLuint>> textures;
        vector<vector<GLuint>> bumpTextures;
        vector<AnimationObject*> animations;
        vector<ShaderGroup*> shaders;
        MaterialCarriage* carriage;
    };
}