#pragma once

namespace ECS2
{
	class ISystemManager;

	struct Context
	{
		Context(EntityManager& entity) : m_entity(entity) {}

		size_t index = 0;
		float deltaTime = 0;
		size_t entityID = 0;

		EntityManager& entity() { return m_entity; };

	private:
		EntityManager& m_entity;
	};
} //namespace ECS2