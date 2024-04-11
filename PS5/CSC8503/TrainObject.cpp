#include "TrainObject.h"
#include "TutorialGame.h"

using namespace NCL::CSC8503;

TrainObject::TrainObject() {

}

TrainObject::~TrainObject() {

}

TrainObject::TrainObject(GameWorld* w) {
	world = w;
	typeID = 20;
	trainCarriage = new TrainCarriage[trainMaxIndex];
	trainIndex = 0;
	name = "Train";
	onFire = false;
	fire = 100.0f;

	distance = 0.0f;
}

void TrainObject::OnCollisionBegin(GameObject* otherObject) {

}

void TrainObject::OnCollisionEnd(GameObject* otherObject) {

}

void TrainObject::UpdateOrientation(Vector3 direction) {
	Quaternion rotation;
	if (direction.x > 0) rotation = Quaternion::EulerAnglesToQuaternion(0, 0, 0);
	else if (direction.x < 0) rotation = Quaternion::EulerAnglesToQuaternion(0, 180, 0);
	else if (direction.z > 0) rotation = Quaternion::EulerAnglesToQuaternion(0, -90, 0);
	else if (direction.z < 0) rotation = Quaternion::EulerAnglesToQuaternion(0, 90, 0);
	transform.SetOrientation(rotation);
}

void TrainObject::Update(float dt) {
	if (path.size() == 0) {
		if (transform.GetPosition() != finishPath) {
			TutorialGame::GetGame()->SetFailure(true);
			return;
		}
		else {
			TutorialGame::GetGame()->SetSuccess(true);
			return;
		}
	}
	else {
		if (path[path.size() - 1] == finalPath)
			path.push_back(finishPath);
		Vector3 target = path[0];
		direction = (target - this->GetTransform().GetPosition());
		direction = Vector3(direction.x, 0, direction.z);
		float trainSpeed = 0.07f;
		force = TutorialGame::GetGame()->GetPlayTime() * trainSpeed + TutorialGame::GetGame()->GetLevel() * 1.0f + 5.0f;
		if (force >= 30.0f)force = 30.0f;
		if (path[path.size() - 1] == finishPath) force = 800.0f;
		GetPhysicsObject()->SetLinearVelocity(Maths::Vector::Normalise(direction) * force * dt);

		float dtdist = Maths::Vector::Length(lastpos - curpos);
		distance += dtdist; //run dist
		lastpos = curpos;
		curpos = this->GetTransform().GetPosition();
		time_s += dt;
		dist_s += dtdist;
		if (time_s >= 1.0f) {
			speed = dist_s / time_s;
			dist_s = time_s = 0.0f;
		}

		float mm = Maths::Vector::Length(this->GetTransform().GetPosition() - target);
		if (mm < 0.5f) {
			if (GetDirection() < 3) transform.SetPosition(Vector3(target.x, transform.GetPosition().y, transform.GetPosition().z));
			else transform.SetPosition(Vector3(transform.GetPosition().x, transform.GetPosition().y, target.z));
			physicsObject->SetLinearVelocity(Vector3());
			path.erase(path.begin());
		}

		if (showInfo) {
			Debug::Print("Train Position: " + std::to_string(transform.GetPosition().x) + " " + std::to_string(transform.GetPosition().y) + " " + std::to_string(transform.GetPosition().z), Vector2(0, 55), Vector4(1, 1, 1, 0.5f));
			Debug::Print("Current Path: " + std::to_string(target.x) + " " + std::to_string(target.y) + " " + std::to_string(target.z), Vector2(0, 60), Vector4(1, 1, 1, 0.5f));
		}
	}
	for (int i = 1; i <= trainIndex; i++)
		trainCarriage[i].Update(dt);
	UpdateOrientation(direction);

	flag1 = onFire;
	if (onFire) {
		float1 = fire;
		float speed = 1.0f;
		if (fire > 0.0f)
			fire -= dt * speed;
		else
			TutorialGame::GetGame()->SetFailure(true);
	}

	showInfo = TutorialGame::GetGame()->ShowInfo();
}

void TrainObject::InitPaths(int level) {
	switch (level) {
	case 1:
		firstPath = Vector3(50, -2.5f, 100);
		finalPath = Vector3(270, -2.5f, 50);
		finishPath = Vector3(290, -2.5f, 50);
		path.push_back(firstPath);
		break;
	case 2:
		firstPath = Vector3(50, -2.5f, 50);
		finalPath = Vector3(270, -2.5f, 150);
		finishPath = Vector3(290, -2.5f, 150);
		path.push_back(firstPath);
		break;
	case 3:
		firstPath = Vector3(50, -2.5f, 20);
		finalPath = Vector3(270, -2.5f, 150);
		finishPath = Vector3(290, -2.5f, 150);
		path.push_back(firstPath);
		break;
	case 4:
		firstPath = Vector3(50, -2.5f, 20);
		finalPath = Vector3(270, -2.5f, 180);
		finishPath = Vector3(290, -2.5f, 180);
		path.push_back(firstPath);
		break;
	case 5:
		firstPath = Vector3(50, -2.5f, 10);
		finalPath = Vector3(270, -2.5f, 160);
		finishPath = Vector3(290, -2.5f, 160);
		path.push_back(firstPath);
		break;
	default:
		break;
	}
}

void TrainObject::AddPath(Vector3 p) {
	path.push_back(Vector3(p.x, -2.5f, p.z));
}

void TrainObject::AddCarriagePath(Vector3 p) {
	TutorialGame::GetGame()->GetMaterialCarriage()->AddPath(p);
	TutorialGame::GetGame()->GetProduceCarriage()->AddPath(p);
	TutorialGame::GetGame()->GetWaterCarriage()->AddPath(p);
}

void TrainObject::UploadAssets(Mesh* mesh, Texture* texture, Shader* shader) {
	carriageMesh = mesh;
	carriageTex = texture;
	basicShader = shader;
}

TrainCarriage* TrainObject::AddCarriage(int id) {
	Vector3 nowPos;
	if (trainIndex == 0)
		nowPos = GetTransform().GetPosition();
	else
		nowPos = trainCarriage[trainIndex].GetTransform().GetPosition();

	Vector3 nextPos;

	nextPos = nowPos;
	nextPos.x -= 12;
	nextPos.y = 4.5f;

	if (id == 21) {
		MaterialCarriage* carriage = new MaterialCarriage(world);
		carriage->SetPath(path);
		AABBVolume* volume = new AABBVolume(Vector3(3, 2, 2));
		carriage->SetBoundingVolume((CollisionVolume*)volume);

		carriage->GetTransform()
			.SetScale(Vector3(4, 4, 4))
			.SetPosition(nextPos);

		carriage->SetRenderObject(new RenderObject(&carriage->GetTransform(), carriageMesh, carriageTex, basicShader));
		carriage->SetPhysicsObject(new PhysicsObject(&carriage->GetTransform(), carriage->GetBoundingVolume()));

		carriage->GetPhysicsObject()->SetInverseMass(0);
		carriage->GetPhysicsObject()->InitSphereInertia();
		carriage->GetPhysicsObject()->SetChannel(1);

		carriage->SetTypeID(id);

		trainCarriage[++trainIndex] = *carriage;
		carriage->SetTrain(this);
		world->AddGameObject(carriage);

		return carriage;
	}
	if (id == 22) {
		ProduceCarriage* carriage = new ProduceCarriage(world);
		carriage->SetPath(path);
		AABBVolume* volume = new AABBVolume(Vector3(3, 2, 2));
		carriage->SetBoundingVolume((CollisionVolume*)volume);

		carriage->GetTransform()
			.SetScale(Vector3(4, 4, 4))
			.SetPosition(nextPos);

		carriage->SetRenderObject(new RenderObject(&carriage->GetTransform(), carriageMesh, carriageTex, basicShader));
		carriage->SetPhysicsObject(new PhysicsObject(&carriage->GetTransform(), carriage->GetBoundingVolume()));

		carriage->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));

		carriage->GetPhysicsObject()->SetInverseMass(0);
		carriage->GetPhysicsObject()->InitSphereInertia();
		carriage->GetPhysicsObject()->SetChannel(1);

		carriage->SetTypeID(id);

		trainCarriage[++trainIndex] = *carriage;
		carriage->SetTrain(this);
		world->AddGameObject(carriage);

		return carriage;
	}
	if (id == 23) {
		WaterCarriage* carriage = new WaterCarriage(world);
		carriage->SetPath(path);
		AABBVolume* volume = new AABBVolume(Vector3(3, 2, 2));
		carriage->SetBoundingVolume((CollisionVolume*)volume);

		carriage->GetTransform()
			.SetScale(Vector3(4, 4, 4))
			.SetPosition(nextPos);

		carriage->SetRenderObject(new RenderObject(&carriage->GetTransform(), carriageMesh, carriageTex, basicShader));
		carriage->SetPhysicsObject(new PhysicsObject(&carriage->GetTransform(), carriage->GetBoundingVolume()));

		carriage->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

		carriage->GetPhysicsObject()->SetInverseMass(0);
		carriage->GetPhysicsObject()->InitSphereInertia();
		carriage->GetPhysicsObject()->SetChannel(1);

		carriage->SetTypeID(id);

		trainCarriage[++trainIndex] = *carriage;
		carriage->SetTrain(this);
		world->AddGameObject(carriage);

		return carriage;
	}
}

int TrainObject::GetDirection() {
	if (direction.x > 0) return 1;
	else if (direction.x < 0) return 2;
	else if (direction.z > 0) return 3;
	else if (direction.z < 0) return 4;
}