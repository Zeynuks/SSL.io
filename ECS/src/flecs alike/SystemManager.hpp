#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include "System.hpp"
#include "EntityManager.hpp"

namespace ECS2
{
	class SystemManager
	{
	public:
		SystemManager(EntityManager& em) : m_entityManager(em) {}

		template<typename... Components>
		ECS2::System<Components...> AddSystem(const std::string name)
		{
			return ECS2::System<Components...>(name, m_entityManager);
		}

	private:
		EntityManager& m_entityManager;
	};
}