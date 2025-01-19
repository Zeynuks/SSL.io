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
		System<Components...>& AddSystem(const std::string& name)
		{
			auto system = std::make_unique<System<Components...>>(name, m_entityManager);
			auto* sysPtr = system.get();
			m_systems.emplace_back(std::move(system));
			return *sysPtr;
		}

		void Update()
		{
			for (const auto& system : m_systems)
			{
				system->Execute();
			}
		}

	private:
		EntityManager& m_entityManager;
		std::vector<std::unique_ptr<ISystem>> m_systems;
	};
} //namespace ECS2