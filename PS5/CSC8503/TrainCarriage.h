#pragma once

#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "StateMachine.h"
#include "NavigationGrid.h"
#include "StateTransition.h"
#include "State.h"
#include "StateMachine.h"
#include "NavigationGrid.h"
#include "GameObject.h"
#include <vector>
#include "RenderObject.h"

namespace NCL::CSC8503 {
    class StoneObject;
    class PlankObject;
    class RailObject;
    class TrainObject;
    class MaterialCarriage;
    class ProduceCarriage;
    class WaterCarriage;
    class TrainCarriage : public GameObject {
    public:
        TrainCarriage();

        ~TrainCarriage();

        void OnCollisionBegin(GameObject* otherObject) override;
        
        void OnCollisionEnd(GameObject* otherObject) override;
        
        void Update(float dt);
        
        void UpdateOrientation();
        int GetDirection();
        
        void AddPath(Vector3 p);
        
        void SetPath(vector<Vector3> p) {
            for (int i = 0; i < p.size(); ++i) {
                path.push_back(Vector3(p[i].x, 4.5f, p[i].z));
            }
        }
        
        void SetTrain(TrainObject* t) {
            train = t;
        }

    protected:
        Vector3 direction;
        GameWorld* world;
        std::vector<Vector3> path;
        TrainObject* train;
    };

    class MaterialCarriage : public TrainCarriage {
    public:
        MaterialCarriage(GameWorld* w) {
            world = w;
            name = "MaterialCarriage";
        };

        ~MaterialCarriage() {};

        void Update(float dt) override;
        
        vector<PlankObject*> GetPlankVector() const {
            return planks;
        }
        vector<StoneObject*> GetStoneVector() const {
            return stones;
        }
        
        void AddStone(StoneObject* stone) {
            stones.push_back(stone);
        }
        void AddPlank(PlankObject* plank) {
            planks.push_back(plank);
        }
        
        bool IsReady() const {
            return ready;
        }
        void SetReady(bool r) {
            ready = r;
        }
        
        ProduceCarriage* GetProduceCarriage() const {
            return produceCarriage;
        }
        void SetProduceCarriage(ProduceCarriage* c) {
            produceCarriage = c;
        }
        
        void UpdateMaterial();

    protected:
        vector<PlankObject*> planks;
        vector<StoneObject*> stones;
        bool ready = false;

        ProduceCarriage* produceCarriage;
    };

    class ProduceCarriage :public TrainCarriage {
    public:
        ProduceCarriage(GameWorld* w) {
            world = w;
            name = "ProduceCarriage";
        };

        ~ProduceCarriage() {};

        void OnCollisionBegin(GameObject* otherObject) override;
       
        void Update(float dt) override;
        
        bool Finished() const {
            return finish;
        }
        void SetFinish(bool f) {
            finish = f;
        }
        
        MaterialCarriage* GetMaterialCarriage() const {
            return materialCarriage;
        }
        void SetMaterialCarriage(MaterialCarriage* c) {
            materialCarriage = c;
        }

    protected:
        vector<RailObject*> rails;
        bool finish = true;
        float counter = 3.0f;

        MaterialCarriage* materialCarriage;
    };
    class WaterCarriage :public TrainCarriage {
    public:
        WaterCarriage(GameWorld* w) {
            world = w;
            name = "WaterCarriage";
        };

        ~WaterCarriage() {};

        void Update(float dt) override;
        void SetCarriageWater(float w) {
            water = w;
        }
        float GetCarriageWater() {
            return water;
        }
    protected:
        float water = 100.0f;
    };
}