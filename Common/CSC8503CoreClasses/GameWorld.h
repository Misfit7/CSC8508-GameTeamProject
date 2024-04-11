#pragma once
#include <random>

#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"
#include "Octree.h"
namespace NCL {
    class Camera;
    using Maths::Ray;
    namespace CSC8503 {
        class GameObject;
        class Constraint;

        typedef std::function<void(GameObject*)> GameObjectFunc;
        typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

        enum GameState {
            LOADING,
            PLAYING,
            SERVERPLAYING,
            CLIENTPLAYING,
            PAUSED,
            FAILURE,
            FINISH,
            MENU,
            CHOOSESERVER,
            EXIT
        };

        class GameWorld {
        public:
            GameWorld();
            ~GameWorld();

            void Clear();
            void ClearAndErase();

            void AddGameObject(GameObject* o);
            void RemoveGameObject(GameObject* o, bool andDelete = false);
            void RemoveGameObject(int id, bool networkedID = false);

            void AddConstraint(Constraint* c);
            void RemoveConstraint(Constraint* c, bool andDelete = false);

            PerspectiveCamera& GetMainCamera() {
                return mainCamera;
            }

            void ShuffleConstraints(bool state) {
                shuffleConstraints = state;
            }

            void ShuffleObjects(bool state) {
                shuffleObjects = state;
            }

            bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false, GameObject* ignore = nullptr, float maxLength = FLT_MAX) const;

            virtual void UpdateWorld(float dt, bool isClient = false);

            void OperateOnContents(GameObjectFunc f);

            void GetObjectIterators(
                GameObjectIterator& first,
                GameObjectIterator& last) const;

            void GetConstraintIterators(
                std::vector<Constraint*>::const_iterator& first,
                std::vector<Constraint*>::const_iterator& last) const;

            void GetRemovedObjectIterators(
                std::vector<int>::const_iterator& first,
                std::vector<int>::const_iterator& last) const;

            int GetWorldStateID() const {
                return worldStateCounter;
            }

            void SetGameState(GameState state) { currentstate = state; };
            GameState GetGameState() { return currentstate; };

        protected:
            std::vector<GameObject*> gameObjects;
            std::vector<Constraint*> constraints;
            std::vector<int> removedNetworkIDs;

            PerspectiveCamera mainCamera;

            bool shuffleConstraints = true;
            bool shuffleObjects = true;
            int		worldIDCounter;
            int		worldStateCounter;

            GameState currentstate;

        };
    }
}

