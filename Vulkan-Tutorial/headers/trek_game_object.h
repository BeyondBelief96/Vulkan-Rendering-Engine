#ifndef TREK_GAME_OBJECT_H
#define TREK_GAME_OBJECT_H

#include "trek_model.h"

//lib
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>


namespace Trek
{
	class TrekGameObject
	{
		struct TransformComponent
		{
			glm::vec3 translation{};
			glm::vec3 scale{ 1.f, 1.f, 1.f };
			glm::vec3 rotation{};
			glm::mat4 mat4();
			glm::mat3 normalMatrix();
		};

	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, TrekGameObject>;

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
		TransformComponent transform2d{};

		std::shared_ptr<TrekModel> model{};
		glm::vec3 color{};
	private:
		explicit TrekGameObject(const id_t objId) : id{objId}{}
		id_t id;
	};
}

#endif
