#pragma once

#include "CollectableObject.h"

namespace NCL::CSC8503 {
	class AxeObject :public CollectableObject {
	public:
		AxeObject(GameWorld* world, std::string name = "") {
			this->name = name;
			this->world = world;
			triggerDelete = true;
			typeID = 3;
			name = "Axe";
		}
		~AxeObject() {};
	};
}