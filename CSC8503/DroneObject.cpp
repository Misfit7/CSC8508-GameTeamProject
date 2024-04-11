#include "DroneObject.h"
#include "TutorialGame.h"

using namespace NCL::CSC8503;

DroneObject::DroneObject(NavigationGrid* navGrid, Vector3 startingPos, GameWorld* world) {
	typeID = 12;
	name = "Drone";
	this->currentPos = startingPos;
	this->startingPos = startingPos;
	this->grid = navGrid;
	this->gridSize = grid->GetGridWidth() * grid->GetGridHeight();
	this->world = world;

	stateCooldown = 5.0f;   // initial value above threshold so doesnt auto detect at start


	detectPlayer = new BehaviourAction("Detect Player", [&](float dt, BehaviourState state)->BehaviourState {
		// if player in collider then success, else fail
		if (threatDetected) {
			//std::cout << "threat detected\n";
			stateCooldown = 0;
			return Success;
		}
		if (stateCooldown < 1.5) {    // keep detecting for few secs after leaving range
			//std::cout << "kept detecting\n";
			return Success;
		}

		else return Failure;
		}
	);

	moveAwayPlayer = new BehaviourAction("Move Away Player", [&](float dt, BehaviourState state)->BehaviourState {
		// always ongoing, success for error
		wanderPathNodes.clear();
		float speed = 20.0f;
		Vector3 direction = (currentPos - threat->GetTransform().GetPosition()).Normalised();
		direction = Vector3(direction.x, 0, direction.z);
		GetPhysicsObject()->SetLinearVelocity(direction * speed);

		float angle = atan2(-direction.x, -direction.z);
		float angleDegrees = Maths::RadiansToDegrees(angle);
		GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), angleDegrees));
		return Ongoing;
		}
	);


	detectItem = new BehaviourAction("Detect Item", [&](float dt, BehaviourState state)->BehaviourState {
		// if item in collider then success, else fail
		if (itemDetected) {
			//std::cout << "item detected\n";
			return Success;
		}
		else return Failure;
		}
	);

	moveTowardItem = new BehaviourAction("Move Toward Item", [&](float dt, BehaviourState state)->BehaviourState {
		// ongoing until item reached, then success

		if (wanderPathNodes.empty()) {
			//std::cout << "pathfind error\n";

			bool pf = Pathfind(item->GetTransform().GetPosition()); // if item not already pathfound to
			if (!pf) {
				std::cout << "Pathfind error\n";
			}
		}

		Vector3 nextPathPos = wanderPathNodes[currentNodeIndex];

		if ((nextPathPos - currentPos).LengthSquared() < 100.0f) { // if close to current node
			if (currentNodeIndex < (wanderPathNodes.size() - 1)) {    // if node isnt the final node, target next node
				currentNodeIndex++;
				nextPathPos = wanderPathNodes[currentNodeIndex];
				timer = 0;
			}
			else { // if final node reached, return success
				return Success;
			}
		}

		MoveToPosition(nextPathPos, 10.0f);  // otherwise move towards next node
		return Ongoing;
		}
	);

	stealItem = new BehaviourAction("Steal Item", [&](float dt, BehaviourState state)->BehaviourState {
		// fail after stealing (to continue root selector), success for error
		//itemDetected = false;
		//std::cout << "item stolen\n";

		TutorialGame::GetGame()->GetWorld()->RemoveGameObject(item);

		return Failure;
		}
	);


	moveOnPatrol = new BehaviourAction("Move On Patrol", [&](float dt, BehaviourState state)->BehaviourState {
		// ongoing, success for error
		GridNode n;
		if (wanderPathNodes.empty()) {
			n = this->grid->GetGridNode(rand() % this->gridSize);
			while (n.type != 0 || !Pathfind(n.position) || (n.position - currentPos).LengthSquared() > 6000.0f) {
				n = grid->GetGridNode(rand() % this->gridSize);
			}
		}

		Vector3 nextPathPos = wanderPathNodes[currentNodeIndex];

		if ((nextPathPos - currentPos).LengthSquared() < 100.0f) { // if close to current node
			if (currentNodeIndex < (wanderPathNodes.size() - 1)) {    // if node isnt the final node, target next node
				currentNodeIndex++;
				nextPathPos = wanderPathNodes[currentNodeIndex];
				timer = 0;
			}
			else { // if final node reached, find new path

				n = grid->GetGridNode(rand() % this->gridSize);
				while (n.type != 0 || !Pathfind(n.position) || (n.position - currentPos).LengthSquared() > 6000.0f) {
					n = grid->GetGridNode(rand() % this->gridSize);
				}
			}
		}

		MoveToPosition(nextPathPos, 10.0f);  // otherwise move towards next node
		return Ongoing;
		}
	);


	playerSequence = new BehaviourSequence("Player Sequence");  // ongoing if player detected, fail if not, success=error
	playerSequence->AddChild(detectPlayer);
	playerSequence->AddChild(moveAwayPlayer);

	itemSequence = new BehaviourSequence("Item Sequence");  // ongoing if item detected, fail if no item anymore, success=error
	itemSequence->AddChild(detectItem);
	itemSequence->AddChild(moveTowardItem);
	itemSequence->AddChild(stealItem);

	patrolSequence = new BehaviourSequence("Patrol Sequence");  // always ongoing, success=error
	patrolSequence->AddChild(moveOnPatrol);

	rootSelector = new BehaviourSelector("Root Selector");  // always ongoing, success=error
	rootSelector->AddChild(playerSequence);
	rootSelector->AddChild(itemSequence);
	rootSelector->AddChild(patrolSequence);

}

void DroneObject::Update(float dt) {
	timer += dt;
	stateCooldown += dt;
	if (timer > 1.0) timer = 1.0;

	renderObject->GetAnimationObject()->SetFrameTime(renderObject->GetAnimationObject()->GetFrameTime() - dt);
	while (renderObject->GetAnimationObject()->GetFrameTime() < 0.0f) {
		renderObject->GetAnimationObject()->SetCurrentFrame((renderObject->GetAnimationObject()->GetCurrentFrame() + 1) %
			renderObject->GetAnimationObject()->GetActiveAnim()->GetFrameCount());
		renderObject->GetAnimationObject()->SetFrameTime(renderObject->GetAnimationObject()->GetFrameTime() + 1.0f /
			renderObject->GetAnimationObject()->GetActiveAnim()->GetFrameRate());
	}

	currentPos = GetTransform().GetPosition();

	bool shouldRespawn = !grid->CheckInGrid(currentPos);
	if (shouldRespawn) {
		//std::cout << "drone respawning\n";
		GetTransform().SetPosition(startingPos);
		currentPos = startingPos;
	}

	if (currentState == Ongoing || currentState == Success) currentState = rootSelector->Execute(dt);   // root selector - do until succeed/ongoing
	if (currentState == Success) std::cout << "Behaviour Tree Error\n";    // error

	/*if (wanderPathNodes.empty()) return;
	for (int i = 0; i < wanderPathNodes.size() - 1; i++)
	{
		Debug::DrawLine(wanderPathNodes[i] + Vector3(0, 1, 0), wanderPathNodes[i + 1] + Vector3(0, 1, 0), Vector4(1, 0, 0, 1));
	}*/
}

void DroneObject::DetectThreat(GameObject* object) {
	//std::cout << "drone detect threat\n";
	if (object->GetTypeID() == 1 || object->GetTypeID() == 11) { // player, teammate
		if ((object->GetTransform().GetPosition() - currentPos).LengthSquared() < 625.0f) {
			threatDetected = true;
			threat = object;
		}
	}
	else if (((object->GetTypeID() == 5 && !object->GetFlag1() && object->GetFlag2()) || (object->GetTypeID() == 6 && object->GetFlag2())) && !itemDetected) { // plank, stone

		if (item != object) {
			bool pf = Pathfind(object->GetTransform().GetPosition()); // if item not already pathfound to
			if (!pf) std::cout << "Pathfind error\n";
		}

		itemDetected = true;
		item = object;
	}
}

void DroneObject::StopDetectThreat(GameObject* object) {
	if (threat == object) {
		threatDetected = false;
	}

	if (item == object) {
		itemDetected = false;
	}
}