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
#include "TrainCarriage.h"
#include "TrainCarriage.h"
#include "PlayerObject.h"
namespace NCL::CSC8503 {
	class StateMachine;
	class RobotObject :public GameObject {
	public:
		RobotObject(NavigationGrid* navGrid, Vector3 position);
		RobotObject() {
			typeID = 11;
			name = "Robot";
		}
		~RobotObject() { delete grid; delete stateMachine; }

		void Update(float dt) override;
		void SetOrigin(Vector3 position) { origin = position; }

		void SetGrid(NavigationGrid* g) {
			grid = g;
		}

		void SetPlayer(PlayerObject* p) {
			player = p;
		}

		bool IsCutting() const {
			return cutting;
		}
		void SetCutting(bool c) {
			cutting = c;
		}
		bool IsDigging() const {
			return digging;
		}
		void SetDigging(bool d) {
			digging = d;
		}

		void OnCollisionBegin(GameObject* otherObject) override;

		int GetStateID() const {
			return stateID;
		}

	protected:
		void Idle(float dt);
		void FollowPlayer(float dt);
		void MoveToTree(float dt);
		void MoveToRock(float dt);
		void CutTree(float dt);
		void DigRock(float dt);
		void MoveToPlayer(float dt);

		GridNode& GetGrid(const Vector3& position);

		void UpdateOrientation(Vector3 direction);

		PlayerObject* player = nullptr;
		Vector3 playerPosition;
		StateMachine* stateMachine;
		NavigationGrid* grid;
		vector<Vector3> nodes;

		float speed = 10.0f;

		float timer = 0;

		Vector3 origin;

		Vector3 targetPosition;

		NavigationPath path;

		Vector3 currentPosition;
		Vector3 face;

		std::string filename;

		Vector3 lastKnownPosition;

		bool stateSwitch;

		Vector3 destPos;

		bool pathNotFound = true;

		Vector3 treePosition;
		Vector3 rockPosition;

		float treeCounter = 0.0f;
		float rockCounter = 0.0f;

		bool cutting = false;
		bool digging = false;
		bool treeFound = false;
		bool rockFound = false;
		bool treeCut = false;
		bool rockDug = false;
		bool nextTree = false;
		bool nextRock = false;

		bool moveToPlayer = false;
		bool foundPlayer = false;

		int stateID = 0;

		int j = 0;
	};
}