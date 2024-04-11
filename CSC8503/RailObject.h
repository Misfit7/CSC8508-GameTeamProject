#pragma once

#include "CollectableObject.h"

namespace NCL::CSC8503 {
	class RailObject :public CollectableObject { 
	public:
		RailObject(GameWorld* world) {
			this->world = world;
			triggerDelete = true;
			typeID = 7;
			placed = false;
			name = "Rail";
		}
		~RailObject() {};

		void SetCarriage(ProduceCarriage* c) {
			carriage = c;
		}

		void SetInCarriage(bool i) {
			inCarriage = i;
		}

		void SetPlaced(bool p) {
			placed = p;
		}

		void SetHeight(float h) {
			height = h;
		}

		int GetNum() const {
			return num;
		}
		void SetNum(int n) {
			num = n;
		}

		void OnCollisionBegin(GameObject* otherObject) override;

		void Update(float dt) override;

		void UploadAssets(Mesh* m, Texture* t, Texture* bt);

		//int GetDirection(Vector3 point1, Vector3 point2);

		//int GetRailDirection(const Vector3& position);

		int GetDirection() const {
			return direction;
		}
		void SetDirection(int d) {
			direction = d;
		}

	protected:
		void PlaceRail();

		bool inCarriage = true;
		bool placed = false;
		float height;
		int num = 1;
		int direction = -1;

		Mesh* railTurnMesh = nullptr;
		Texture* railTurnTex = nullptr;
		Texture* railTurnBumpTex = nullptr;

		ProduceCarriage* carriage = nullptr;
	};
}