#ifndef TREK_GAME_OBJECT_H
#define TREK_GAME_OBJECT_H
#include <memory>

#include "trek_model.h"

namespace Trek
{
	class TrekGameObject
	{
		struct Transform2dComponent
		{
			glm::vec2 translation{};
			glm::vec2 scale{ 1.f, 1.f };
			float rotation;
			glm::mat2 mat2() const
			{
				const float s = glm::sin(rotation);
				const float c = glm::cos(rotation);
				const glm::mat2 rotMatrix{ {c, s,},{-s, c } };
				const glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
				return rotMatrix * scaleMat;
			}
		};

	public:
		using id_t = unsigned int;

		TrekGameObject(const TrekGameObject&) = delete;
		TrekGameObject& operator=(TrekGameObject&) = delete;
		TrekGameObject(TrekGameObject&& obj) = default;
		TrekGameObject& operator=(TrekGameObject&& obj) = default;

		static TrekGameObject createGameObject()
		{
			static id_t currentId = 0;
			return TrekGameObject(currentId++);
		}

		id_t getId() const { return id; }
		Transform2dComponent transform2d{};

		std::shared_ptr<TrekModel> model{};
		glm::vec3 color{};
	private:
		explicit TrekGameObject(const id_t objId) : id{objId}{}
		id_t id;
	};
}

#endif