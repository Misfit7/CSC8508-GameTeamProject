#pragma once
#include "RenderObject.h"

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		class GameObject;
		class NetworkState	{ 
		public:
			NetworkState();
			virtual ~NetworkState();

			int GetStateID() const {
				return stateID;
			}

			Vector3		   position;
			Quaternion	   orientation;
			Vector3        scale;
			Vector4        colour;
			int            currentFrame;
			int			   stateID;
			int            slot;
			int            slotNum;
			bool           flag1;
			float          float1;
			float          float2;
			float          float3;
			Vector3        position1;
		};
	}
}

