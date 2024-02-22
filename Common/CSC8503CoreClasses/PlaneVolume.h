#pragma once
#include "CollisionVolume.h"
#include "Vector2.h"
namespace NCL {
	using namespace Maths;
	class PlaneVolume : CollisionVolume
	{
	public:
		PlaneVolume(const Vector2& size) {
			type = VolumeType::Plane;
			planeSize = size;
		}
		~PlaneVolume() {

		}

		Vector2 GetPlaneSize() const {
			return planeSize;
		}

	protected:
		Vector2 planeSize;
	};
}
