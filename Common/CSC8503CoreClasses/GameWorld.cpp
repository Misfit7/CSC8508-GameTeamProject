#include "GameWorld.h"
#include "GameObject.h"
#include "Constraint.h"
#include "CollisionDetection.h"
#include "Camera.h"
#include "NetworkObject.h"
#include "Window.h"

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld() {
    std::cout << std::endl << "--------Initialising Game World--------" << std::endl;

    shuffleConstraints = false;
    shuffleObjects = false;
    worldIDCounter = 0;
    worldStateCounter = 0;
}

GameWorld::~GameWorld() {
}

void GameWorld::Clear() {
    gameObjects.clear();
    constraints.clear();
    worldIDCounter = 0;
    worldStateCounter = 0;
}

void GameWorld::ClearAndErase() {
    for (auto& i : gameObjects) {
        delete i;
    }
    for (auto& i : constraints) {
        delete i;
    }
    Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
    gameObjects.emplace_back(o);
    o->SetWorldID(worldIDCounter++);
    worldStateCounter++;
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete) {
    removedNetworkIDs.push_back(o->GetNetworkObject()->GetNetworkID());
    gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
    if (andDelete) {
        delete o;
    }
    worldStateCounter++;
}

void GameWorld::RemoveGameObject(int id, bool networkID) {
    if (!networkID) {
        for (auto& i : gameObjects) {
            if (i->GetWorldID() == id)
                RemoveGameObject(i, false);
        }
    }
    else {
        for (auto& i : gameObjects) {
            if (i->GetNetworkObject()) {
                if (i->GetNetworkObject()->GetNetworkID() == id)
                    RemoveGameObject(i, false);
            }
        }
    }
}

void GameWorld::GetObjectIterators(
    GameObjectIterator& first,
    GameObjectIterator& last) const {

    first = gameObjects.begin();
    last = gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
    for (GameObject* g : gameObjects) {
        f(g);
    }
}

void GameWorld::UpdateWorld(float dt, bool isClient) {
    if (!isClient) {
        auto rng = std::default_random_engine{};

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine e(seed);

        if (shuffleObjects) {
            std::shuffle(gameObjects.begin(), gameObjects.end(), e);
        }

        if (shuffleConstraints) {
            std::shuffle(constraints.begin(), constraints.end(), e);
        }

        for (auto& i : gameObjects) {
            if (i->IsActive()) {
                i->Update(dt);
            }
            else {
                RemoveGameObject(i, false);
            }
        }
    }
    else {
        for (auto& i : gameObjects) {
            if (i->IsActive()) {
                if (i->UpdateInClient()) {
                    i->Update(dt);
                }
            }
        }
    }
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject, GameObject* ignoreThis, float maxLength) const {
    //The simplest raycast just goes through each object and sees if there's a collision
     RayCollision collision;

    for (auto& i : gameObjects) {
        if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
            continue;
        }
        if (i == ignoreThis) {
            continue;
        }
        if (i->GetTypeID() == 131) {
            continue;
        }
        RayCollision thisCollision;
        if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {
            if (thisCollision.rayDistance < maxLength) {
                if (!closestObject) {
                    closestCollision = collision;
                    closestCollision.node = i;
                    return true;
                }
                else {
                    if (thisCollision.rayDistance < collision.rayDistance) {
                        thisCollision.node = i;
                        collision = thisCollision;
                    }
                }
            }
        }
    }
    if (collision.node) {
        closestCollision = collision;
        closestCollision.node = collision.node;
        return true;
    }
    return false;
}


/*
Constraint Tutorial Stuff
*/

void GameWorld::AddConstraint(Constraint* c) {
    constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c, bool andDelete) {
    constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
    if (andDelete) {
        delete c;
    }
}

void GameWorld::GetConstraintIterators(
    std::vector<Constraint*>::const_iterator& first,
    std::vector<Constraint*>::const_iterator& last) const {
    first = constraints.begin();
    last = constraints.end();
}

void GameWorld::GetRemovedObjectIterators(
    std::vector<int>::const_iterator& first,
    std::vector<int>::const_iterator& last) const {
    first = removedNetworkIDs.begin();
    last = removedNetworkIDs.end();
}