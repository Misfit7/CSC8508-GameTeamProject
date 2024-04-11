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
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "AnimalObject.h"

namespace NCL::CSC8503 {
	class DroneObject :public AnimalObject {
	public:
		DroneObject(NavigationGrid* navGrid, Vector3 startingPos, GameWorld* world);
		~DroneObject() {};

		void Update(float dt) override;

		void DetectThreat(GameObject* object) override;
		void StopDetectThreat(GameObject* object) override;

	protected:
		BehaviourState currentState = Ongoing;
		BehaviourSelector* rootSelector;

		BehaviourSequence* playerSequence;
		BehaviourAction* detectPlayer;
		BehaviourAction* moveAwayPlayer;

		BehaviourSequence* itemSequence;
		BehaviourAction* detectItem;
		BehaviourAction* moveTowardItem;
		BehaviourAction* stealItem;

		BehaviourSequence* patrolSequence;
		BehaviourAction* moveOnPatrol;

		bool itemDetected = false;
		GameObject* item = nullptr;
	};
}