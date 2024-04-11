#pragma once

#include "CollectableObject.h"

namespace NCL::CSC8503 {
	class PlankObject :public CollectableObject {
	public:
		PlankObject(GameWorld* world) {
			this->world = world;
			triggerDelete = true;
			typeID = 5;
			name = "Plank";
		}
		~PlankObject() {};

		void OnCollisionBegin(GameObject* otherObject) override;

		void Update(float dt) override;

		void CreateBridge();

		void SetHeight(float h) {
			height = h;
		}

	protected:
		bool inCarriage = false;
		bool loading = false;
		bool placed = false;
		MaterialCarriage* carriage = nullptr;
		float height;
	};
}