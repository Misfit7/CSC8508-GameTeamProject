#include "AnimalObject.h"

using namespace NCL::CSC8503;

AnimalObject::AnimalObject(NavigationGrid* navGrid, Vector3 startingPos, GameWorld* world) {
    this->typeID = 10;
    this->stateMachine = new StateMachine();
    this->currentPos = startingPos;
    this->grid = navGrid;
    this->gridSize = grid->GetGridWidth() * grid->GetGridHeight();
    this->world = world;
    this->name = "Moose";

    GridNode n = grid->GetGridNode(rand() % this->gridSize);
    while (n.type != 0 || !Pathfind(n.position) || pow((Maths::Vector::Length(n.position - currentPos)), 2) > 6000.0f) {
        n = grid->GetGridNode(rand() % this->gridSize);
    }

    timer = 0;

    State* wander = new State([&](float dt) -> void {
        //std::cout << "enter wander state" << std::endl;
        if (wanderPathNodes.empty()) {
            n = grid->GetGridNode(rand() % this->gridSize);
            while (n.type != 0 || !Pathfind(n.position) || pow((Maths::Vector::Length(n.position - currentPos)), 2) > 6000.0f) {
                n = grid->GetGridNode(rand() % this->gridSize);
            }
        }

        Vector3 nextPathPos = wanderPathNodes[currentNodeIndex];

        if (pow((Maths::Vector::Length(nextPathPos - currentPos)), 2) < 100.0f) { // if close to current node
            if (currentNodeIndex < (wanderPathNodes.size() - 1)) {    // if node isnt the final node, target next node
                currentNodeIndex++;
                nextPathPos = wanderPathNodes[currentNodeIndex];
                timer = 0;
            }
            else { // if final node reached, find new path
              
                n = grid->GetGridNode(rand() % this->gridSize);
                while (n.type != 0 || !Pathfind(n.position) || pow((Maths::Vector::Length(n.position - currentPos)), 2) > 6000.0f) {
                    n = grid->GetGridNode(rand() % this->gridSize);
                }
            }
        }

        MoveToPosition(nextPathPos, 10.0f);  // otherwise move towards next node

        });

    stateMachine->AddState(wander);


    State* scared = new State([&](float dt) -> void {
        float speed = 20.0f;
        Vector3 direction = Maths::Vector::Normalise(currentPos - threat->GetTransform().GetPosition());
        direction = Vector3(direction.x, 0, direction.z);
        GetPhysicsObject()->SetLinearVelocity(direction * speed);

        float angle = atan2(-direction.x, -direction.z);
        float angleDegrees = Maths::RadiansToDegrees(angle);
        GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), angleDegrees));

        });

    stateMachine->AddState(scared);




    stateMachine->AddTransition(new StateTransition(wander, scared, [&]() -> bool { // transition to scared state if runs into player/enemy/etc

        if (threatDetected && stateCooldown > 1.0) {
            //std::cout << "entering scared state\n";
            stateCooldown = 0;
            return true;
        }
        return false;

        }));


    stateMachine->AddTransition(new StateTransition(scared, wander, [&]() -> bool {

        if (!threatDetected && stateCooldown > 1.0) {
            //std::cout << "exiting scared state\n";
            stateCooldown = 0;
            threat = nullptr;

            GridNode n = grid->GetGridNode(rand() % this->gridSize);
            while (n.type != 0 || !Pathfind(n.position) || pow((Maths::Vector::Length(n.position - currentPos)), 2) > 6000.0f) {
                n = grid->GetGridNode(rand() % this->gridSize);
            }

            return true;
        }
        return false;

        }));
}

void AnimalObject::Update(float dt) {
    timer += dt;
    stateCooldown += dt;
    if (timer > 1.0) timer = 1.0;

    renderObject->UpdateAnimation(dt);

    currentPos = GetTransform().GetPosition();

    stateMachine->Update(dt);

    /*if (wanderPathNodes.empty()) return;
    for (int i = 0; i < wanderPathNodes.size() - 1; i++)
    {
        Debug::DrawLine(wanderPathNodes[i] + Vector3(0, 1, 0), wanderPathNodes[i + 1] + Vector3(0, 1, 0), Vector4(1, 0, 0, 1));
    }*/
}

void AnimalObject::MoveToPosition(Vector3 targetPos, float speed) {
    Vector3 direction = Maths::Vector::Normalise(targetPos - currentPos);
    GetPhysicsObject()->SetLinearVelocity(Vector3(0, GetPhysicsObject()->GetLinearVelocity().y, 0) + direction * speed);

    float angle = atan2(-direction.x, -direction.z);
    float angleDegrees = Maths::RadiansToDegrees(angle);
    GetTransform().SetOrientation(Quaternion::Lerp(GetTransform().GetOrientation(), Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), angleDegrees), timer));
}

bool AnimalObject::Pathfind(Vector3 targetPos) { // pathfinds to target position
    wanderPathNodes = {};
    currentNodeIndex = 0;

    NavigationPath outPath;
    bool foundPath = grid->FindPath(currentPos, targetPos, outPath);

    Vector3 pos;
    while (outPath.PopWaypoint(pos)) {  // converts path into Vector3 position nodes
        wanderPathNodes.push_back(pos);
    }

    return foundPath;
}

void AnimalObject::DetectThreat(GameObject* object) {
    //std::cout << "detected " << object->GetName() << std::endl;           

    if (object->GetTypeID() == 1 || object->GetTypeID() == 11 || object->GetTypeID() == 12 || object->GetTypeID() == 20) { // player, teammate, enemy, train
        threatDetected = true;
        threat = object;
    }
}

void AnimalObject::StopDetectThreat(GameObject* object) {
    //std::cout << "stopped detecting " << object->GetName() << std::endl;
    if (threat == object) {
        threatDetected = false;
    }
}





DetectionSphereObject::DetectionSphereObject(AnimalObject* animal) {
    this->animal = animal;
}