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

namespace NCL::CSC8503 {
	class TrainObject : public GameObject {

	public:
		TrainObject();
		TrainObject(GameWorld* w);
		~TrainObject();

		void OnCollisionBegin(GameObject* otherObject) override;

		void OnCollisionEnd(GameObject* otherObject) override;

		void Update(float dt);

		void InitPaths(int level);

		void AddPath(Vector3 p);

		void AddCarriagePath(Vector3 p);

		TrainCarriage* AddCarriage(int id, bool spawn = true);

		void AddConstraint(GameObject* a, GameObject* b);

		void UpdateOrientation(Vector3 direction);

		void UploadAssets(Mesh* mesh, Texture* texture, ShaderGroup* shader);

		bool OnFire() const {
			return onFire;
		}
		void SetOnFire(bool o) {
			onFire = o;
		}

		float GetFire() const {
			return fire;
		}
		void SetFire(float f) {
			fire = f;
		}

		float GetForce() { return force; }
		float GetDistance() { return distance; }
		void SetDistance(float d) { distance = d; }
		float GetSpeed() { return speed; }
		Vector3 GetLastPath() { return path[path.size() - 1]; }
		void SetInitPos(Vector3 pos1) { lastpos = curpos = pos1; }

	protected:
		int GetDirection();

		//0  1  2 3 up down left right
		std::vector<Vector3> path;
		TrainCarriage* trainCarriage;
		bool onFire = false;
		float fire = 100.0f;
		int trainMaxIndex = 20;
		int trainIndex = 0;
		GameWorld* world;
		Mesh* carriageMesh = nullptr;
		Texture* carriageTex = nullptr;
		ShaderGroup* basicShader = nullptr;
		Vector3 direction;
		Vector3 firstPath;
		Vector3 finalPath;
		Vector3 finishPath;

		//UI
		float force = 10.0f;
		Vector3 lastpos = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 curpos = Vector3(0.0f, 0.0f, 0.0f);
		float distance = 0.0f;
		float speed = 0.0f;
		float dist_s = 0.0f;
		float time_s = 0.0f;
	};
}


