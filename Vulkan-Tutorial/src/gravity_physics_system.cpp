#include "gravity_physics_system.h"

#include "pfgen.h"

namespace Trek
{
	GravityPhysicsSystem::GravityPhysicsSystem(std::vector<Trek::TrekGameObject>& physicsObjects)
		: physicsObjects{ physicsObjects }
	{
		for (const auto& obj : physicsObjects)
		{
			physicsWorld.AddParticle2D(obj.particle);
		}
	}

	void GravityPhysicsSystem::update(const float dt)
	{
		for (auto iterA = physicsObjects.begin(); iterA != physicsObjects.end(); ++iterA)
		{
			auto& objA = *iterA;
			for (auto iterB = iterA; iterB != physicsObjects.end(); ++iterB)
			{
				if (iterA == iterB) continue;
				auto& objB = *iterB;
				auto force = computeGravitationalForce(objA, objB);
				objA.particle->ApplyForce(force);
				objB.particle->ApplyForce(-force);
			}
		}

		physicsWorld.Update(dt);
		for(auto& obj : physicsObjects)
		{
			obj.transform2d.translation = ConvertAeroToGlm(obj.particle->position * 0.7);
		}
	}

	AeroVec2 GravityPhysicsSystem::computeGravitationalForce(const TrekGameObject& fromObj, const TrekGameObject& toObj) const
	{
		const auto offset = fromObj.transform2d.translation - toObj.transform2d.translation;
		const float distanceSquared = glm::dot(offset, offset);

		// clown town - just going to return 0 if objects are too close together...
		if (glm::abs(distanceSquared) < 0.005) {
			return { .0f, .0f };
		}

		if(fromObj.particle != nullptr && toObj.particle != nullptr)
		{
			auto force = Particle2DForceGenerators::GenerateGravitationalAttractionForce(
				*fromObj.particle,
				*toObj.particle,
				0,
				1000,
				gravitationalConstant);

			return force;
		}

		// This case is only needed to calculate the force at different points in the
		// vector field (so not particles, so we don't need to use the physics system for
		// this).
		const float force =
			gravitationalConstant * 1 * 1 / distanceSquared;
		const auto glmforceVec = force * offset / glm::sqrt(distanceSquared);
		return AeroVec2(glmforceVec.x, glmforceVec.y);
	}
}


