#pragma once

#include <string>
#include <vector>
#include <typeindex>

#include "EntityManager.hpp"

namespace ECS2
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;
		virtual void Execute() = 0;
	};

	template<typename... Components>
	class System : public ISystem
	{
	public:
		System(const std::string& name, EntityManager& entityManager)
			: m_name(name), m_entityManager(entityManager) {
		}

		void Each(std::function<void(Components&...)> callback)
		{
			m_callback = std::move(callback);
		}

		template<typename Component>
		System& With()
		{
			m_filters.push_back(typeid(Component));
			return *this;
		}

		void Execute() override
		{
			if (!m_callback)
			{
				return;
			}

			for (auto* entity : m_entityManager.GetAll())
			{
				if (HasFilters(*entity) && HasAllComponents<Components...>(*entity))
				{
					m_callback(*entity->Get<Components>()...);
				}
			}
		}

	private:
		std::string m_name;
		std::function<void(Components&...)> m_callback;
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
} //namespace ECS2