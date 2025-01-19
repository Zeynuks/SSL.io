#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>

#include "System.hpp"
#include "EntityManager.hpp"

namespace detail
{
	template<typename Callback, typename... Components, std::size_t... Is>
	void invoke_with_components_impl(Callback& callback, std::vector<void*>& rawComponents, std::index_sequence<Is...>)
	{
		callback(*static_cast<Components*>(rawComponents[Is])...);
	}

	template<typename... Components, typename Callback>
	void invoke_with_components(Callback& callback, std::vector<void*>& rawComponents)
	{
		invoke_with_components_impl<Callback, Components...>(callback, rawComponents, std::make_index_sequence<sizeof...(Components)>());
	}
} // namespace detail

namespace ECS2
{
	class ISystemManager
	{
	public:
		virtual ~ISystemManager() = default;
		virtual std::vector<std::unique_ptr<System>>& GetSystems() = 0;
	};

	class SystemManager : private ISystemManager
	{
	public:
		SystemManager(EntityManager& em) : m_entityManager(em) {}

		template<typename... Components>
		auto AddSystem(const std::string& name)
		{
			return SystemBuilder<Components...>(*this, name);
		}

		void Update() {
			for (const auto& system : m_systems)
			{
				for (auto* entity : m_entityManager.GetAll())
				{
					if (!entity->IsValid()) continue;

					std::vector<void*> components;
					components.reserve(system->GetFilters().size());
					bool hasAll = true;
					for (const auto& filter : system->GetFilters())
					{
						if (entity->Has(filter))
						{
							components.push_back(entity->Get(filter));
						}
						else
						{
							hasAll = false;
							break;
						}
					}
					if (hasAll && components.size() == system->GetFilters().size())
					{
						system->GetCallback()(components);
					}
				}
			}
		}

	private:
		EntityManager& m_entityManager;
		std::vector<std::unique_ptr<System>> m_systems;

		std::vector<std::unique_ptr<System>>& GetSystems() override
		{
			return m_systems;
		}
	};

	template<typename... Components>
	class SystemBuilder
	{
	public:
		SystemBuilder(ISystemManager& manager, const std::string& name) : m_manager(manager), m_system(name)
		{
			(m_system.AddFilter(typeid(Components)), ...);
		}

		template<typename Component>
		SystemBuilder& With()
		{
			m_system.AddFilter(typeid(Component));
			return *this;
		}

		ISystemManager& Each(std::function<void(Components&...)> callback)
		{
			m_system.SetCallback([callback](std::vector<void*>& rawComponents)
				{
					if constexpr (sizeof...(Components) > 0)
					{
						detail::invoke_with_components<Components...>(callback, rawComponents);
					}
					else
					{
						callback();
					}
				});
			m_manager.GetSystems().push_back(std::make_unique<System>(std::move(m_system)));
			return m_manager;
		}

	private:
		System m_system;
		ISystemManager& m_manager;
	};
} // namespace ECS2