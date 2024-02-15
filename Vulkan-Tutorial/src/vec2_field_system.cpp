#include "vec2_field_system.h"

namespace Trek
{
	void Vec2FieldSystem::update(
        const GravityPhysicsSystem& physicsSystem,
        const std::vector<Trek::TrekGameObject>& physicsObjs,
        std::vector<TrekGameObject>& vectorField)
	{
        for (auto& vf : vectorField) {
            glm::vec2 direction{};
            for (auto& obj : physicsObjs) {
                direction += ConvertAeroToGlm(physicsSystem.computeGravitationalForce(obj, vf));
            }

            // This scales the length of the field line based on the log of the length
            // values were chosen just through trial and error based on what i liked the look
            // of and then the field line is rotated to point in the direction of the field
            vf.transform2d.scale.x =
                0.005f + 0.045f * glm::clamp(glm::log(glm::length(direction) + 1) / 3.f, 0.f, 1.f);
            vf.transform2d.rotation = atan2(direction.y, direction.x);
        }
	}
}
