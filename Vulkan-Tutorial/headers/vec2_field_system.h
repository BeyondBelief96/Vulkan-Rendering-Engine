#ifndef VEC2_FIELD_SYSTEM_H
#define VEC2_FIELD_SYSTEM_H
#include "gravity_physics_system.h"

namespace Trek
{
	class Vec2FieldSystem
	{
	public:
		static void update(
			const GravityPhysicsSystem& physicsSystem,
			const std::vector<Trek::TrekGameObject>& physicsObjs,
			std::vector <TrekGameObject>& vectorField);
	};
}

#endif
