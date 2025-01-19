#pragma once
#include <string>
#include <vector>
#include <typeindex>

#include "EntityManager.hpp"

namespace ECS2
{
	template<typename... Components>
	class System
	{
	public:
		System(const std::string& name, EntityManager& entityManager)
			: m_name(name), m_entityManager(entityManager) {
		}

		void Each(std::function<void(Components&...)> callback)
		{
			for (auto* entity : m_entityManager.GetAll())
			{
				if (HasFilters(*entity) && HasAllComponents<Components...>(*entity))
				{
					callback(*entity->Get<Components>()...);
				}
			}
		}

		template<typename Component>
		System& With()
		{
			m_filters.push_back(typeid(Component));
			return *this;
		}
	private:
		std::string m_name;
		EntityManager& m_entityManager;
		std::vector<std::type_index> m_filters;

		bool HasFilters(Entity& entity)
		{
			for (const auto& filter : m_filters) {
				if (entity.Has(filter))
				{
					continue;
				}
				return false;
			}
			return true;
		}

		template<typename... Components>
		bool HasAllComponents(const Entity& entity)
		{
			return (entity.Has<Components>() && ...);
		}
	};
}