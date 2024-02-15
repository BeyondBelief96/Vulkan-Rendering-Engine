#ifndef GRAVITY_PHYSICS_SYSTEM_H
#define GRAVITY_PHYSICS_SYSTEM_H
#include "AeroWorld2D.h"
#include "trek_game_object.h"

namespace Trek
{
	class GravityPhysicsSystem
	{
	public:
		explicit GravityPhysicsSystem(std::vector<TrekGameObject>& physicsObjects);
		void update(float dt);
		AeroVec2 computeGravitationalForce(const TrekGameObject& fromObj, const TrekGameObject& toObj) const;
	private:
		std::vector<TrekGameObject>& physicsObjects;
		AeroWorld2D physicsWorld{ 0 };
		float gravitationalConstant = 0.08;
	};
}



#endif
