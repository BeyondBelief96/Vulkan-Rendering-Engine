#include "keyboard_movement_controller.h"

namespace Trek
{
	void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, TrekGameObject& gameObject) const
	{
		glm::vec3 rotate{ 0.f };
		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;


		if(glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		{
			gameObject.transform2d.rotation += turnSpeed * dt * glm::normalize(rotate);
		}

		gameObject.transform2d.rotation.x = 
			glm::clamp(gameObject.transform2d.rotation.x, -1.5f, 1.5f);

		gameObject.transform2d.rotation.y =
			glm::mod(gameObject.transform2d.rotation.y, glm::two_pi<float>());

		const float yaw = gameObject.transform2d.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
		{
			gameObject.transform2d.translation += moveSpeed * dt * glm::normalize(moveDir);
		}
	}
}
