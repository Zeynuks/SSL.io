#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>

#include "System.hpp"
#include "Context.hpp"
#include "EntityManager.hpp"

namespace ECS2
{
	namespace detail
	{
		template<typename Callback, typename... Components, std::size_t... Is>
		void invoke_with_components_impl(Callback&& callback, Context& context, std::vector<void*>& rawComponents, std::index_sequence<Is...>)
		{
			std::invoke(std::forward<Callback>(callback), context, *static_cast<Components*>(rawComponents[Is])...);
		}

		template<typename... Components, typename Callback>
		void invoke_with_components(Callback&& callback, Context& context, std::vector<void*>& rawComponents)
		{
			invoke_with_components_impl<Callback, Components...>(std::forward<Callback>(callback), context, rawComponents, std::make_index_sequence<sizeof...(Components)>());
		}
	} // namespace detail

	class ISystemManager
	{
	public:
		virtual ~ISystemManager() = default;
		virtual std::vector<std::unique_ptr<System>>& GetSystems() = 0;
	};

	class SystemManager : private ISystemManager
	{
	public:
		SystemManager(EntityManager& em) 
			: m_entityManager(em)
			, m_context(*this, em) {}

		template<typename... Components>
		auto AddSystem(const std::string& name)
		{
			return SystemBuilder<Components...>(*this, name);
		}

		void Update()
		{
			for (const auto& system : m_systems)
			{
				for (size_t entityIndex = 0; entityIndex < m_entityManager.GetAll().size(); ++entityIndex)
				{
					auto* entity = m_entityManager.GetAll()[entityIndex];
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
						m_context.index = entityIndex;
						m_context.entityID = entity->GetID();
						system->GetCallback()(m_context, components);
					}
				}
			}
		}

	private:
		EntityManager& m_entityManager;
		std::vector<std::unique_ptr<System>> m_systems;
		Context m_context;

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

		template<typename Callback>
		ISystemManager& Each(Callback&& callback)
		{
			m_system.SetCallback([callback = std::forward<Callback>(callback)](Context& context, std::vector<void*>& rawComponents)
				{
					if constexpr (sizeof...(Components) > 0)
					{
						detail::invoke_with_components<Components...>(
							[&callback](Context& inner_context, Components&... components) {
								(void)inner_context;
								std::invoke(callback, components...);
							},
							context, rawComponents);
					}
					else
					{
						std::invoke(callback);
					}
				});
			m_manager.GetSystems().push_back(std::make_unique<System>(std::move(m_system)));
			return m_manager;
		}

		template<typename Callback>
		ISystemManager& Each(Callback&& callback, bool context_needed)
		{
			m_system.SetCallback([callback = std::forward<Callback>(callback)](Context& context, std::vector<void*>& rawComponents)
				{
					if constexpr (sizeof...(Components) > 0)
					{
						detail::invoke_with_components<Components...>(
							[&callback](Context& inner_context, Components&... components) {
								std::invoke(callback, inner_context, components...);
							},
							context, rawComponents);
					}
					else
					{
						std::invoke(callback, context);
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