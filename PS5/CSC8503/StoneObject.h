#pragma once

#include "CollectableObject.h"

namespace NCL::CSC8503 {
	class StoneObject :public CollectableObject {
	public:
		StoneObject(GameWorld* world) {
			this->world = world;
			triggerDelete = true;
			typeID = 6;
			name = "Stone";
		}
		~StoneObject() {};

		void OnCollisionBegin(GameObject* otherObject) override;

		void Update(float dt) override;

		void SetHeight(float h) {
			height = h;
		}

	protected:
		bool inCarriage = false;
		bool loading = false;
		MaterialCarriage* carriage = nullptr;
		float height;
	};
}