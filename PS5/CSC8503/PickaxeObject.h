#pragma once

#include "CollectableObject.h"

namespace NCL::CSC8503 {
	class PickaxeObject :public CollectableObject {
	public:
		PickaxeObject(GameWorld* world, std::string name = "") {
			this->name = name;
			this->world = world;
			triggerDelete = true;
			typeID = 2;
			name = "Pickaxe";
		}
		~PickaxeObject() {};
	};
}