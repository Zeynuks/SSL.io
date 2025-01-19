#pragma once

namespace ECS2
{
	class ISystemManager;

	struct Context
	{
		Context(ISystemManager& system, EntityManager& entity)
			: m_system(system), m_entity(entity) {
		}

		size_t index = 0;
		float deltaTime = 0;
		size_t entityID = 0;

		ISystemManager& system() { return m_system; };
		EntityManager& entity() { return m_entity; };

	private:
		ISystemManager& m_system;
		EntityManager& m_entity;
	};
} //namespace ECS2