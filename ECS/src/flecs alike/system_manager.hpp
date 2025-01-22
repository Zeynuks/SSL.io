#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>

#include "system.hpp"
#include "context.hpp"
#include "entity_manager.hpp"

namespace ecs
{
	namespace detail
	{
		template<typename callback_fn, typename... components, std::size_t... Is>
		void invoke_with_components_impl(callback_fn&& callback, context& context, std::vector<void*>& rawComponents, std::index_sequence<Is...>)
		{
			std::invoke(std::forward<callback_fn>(callback), context, *static_cast<components*>(rawComponents[Is])...);
		}

		template<typename... components, typename callback_fn>
		void invoke_with_components(callback_fn&& callback, context& context, std::vector<void*>& rawComponents)
		{
			invoke_with_components_impl<callback_fn, components...>(std::forward<callback_fn>(callback), context, rawComponents, std::make_index_sequence<sizeof...(components)>());
		}
	} // namespace detail

	class system_manager
	{
	public:
		virtual ~system_manager() = default;
		virtual std::vector<std::unique_ptr<system_impl>>& get_systems() = 0;
	};

	class system_manager_impl : private system_manager
	{
	public:
		system_manager_impl(entity_manager& em)
			: m_entityManager(em)
			, m_context(em) {
		}

		template<typename... components>
		auto system(const std::string& name)
		{
			return system_builder<components...>(*this, name);
		}

		void update(float delta_time)
		{
			auto [begin, end] = m_entityManager.get_all_entities();
			if (begin == end)
			{
				return;
			}
			for (auto& entity = begin; entity != end; ++entity)
			{
				if (!entity->is_valid()) continue;

				for (const auto& system : m_systems)
				{
					std::vector<void*> components;
					components.reserve(system->get_filters().size());
					bool hasAll = true;
					for (const auto& filter : system->get_filters())
					{
						if (entity->has(filter))
						{
							components.push_back(entity->get(filter));
						}
						else
						{
							hasAll = false;
							break;
						}
					}
					if (hasAll && components.size() == system->get_filters().size())
					{
						m_context.index = entity.GetIndex();
						m_context.ID = entity->ID();
						m_context.delta_time = delta_time;
						system->get_callback()(m_context, components);
					}
				}
			}
		}

	private:
		entity_manager& m_entityManager;
		std::vector<std::unique_ptr<system_impl>> m_systems;
		context m_context;

		std::vector<std::unique_ptr<system_impl>>& get_systems() override
		{
			return m_systems;
		}
	};

	template<typename... components>
	class system_builder
	{
	public:
		system_builder(system_manager& manager, const std::string& name) : m_manager(manager), m_system(name)
		{
			(m_system.add_filter(typeid(components)), ...);
		}

		template<typename component>
		system_builder& with()
		{
			m_system.add_filter(typeid(component));
			return *this;
		}

		template<typename callback_fn>
		system_manager& each(callback_fn&& callback)
		{
			return set_each_callback([callback = std::forward<callback_fn>(callback)](context& _context, std::vector<void*>& rawComponents)
				{
					if constexpr (sizeof...(components) > 0)
						detail::invoke_with_components<components...>(
							[&callback](context& inner_context, components&... components) {
								(void)inner_context;
								std::invoke(callback, components...);
							},
							_context, rawComponents);
					else
						std::invoke(callback);
				});
		}

		template<typename T, typename method>
		system_manager& each(method _method, T& instance)
		{
			return set_each_callback([method = std::forward<method>(_method), &instance](context& _context, std::vector<void*>& rawComponents)
				{
					if constexpr (sizeof...(components) > 0)
						detail::invoke_with_components<components...>(
							[&method, &instance](context& inner_context, components&... components)
							{
								(void)inner_context;
								(instance.*method)(components...);
							},
							_context, rawComponents);
					else
						std::invoke(method);
				});
		}

		template<typename T, typename method>
		system_manager& each(method _method, T& instance, bool context_needed)
		{
			return set_each_callback([method = std::forward<method>(_method), &instance](context& _context, std::vector<void*>& rawComponents)
				{
					if constexpr (sizeof...(components) > 0)
						detail::invoke_with_components<components...>(
							[&method, &instance](context& inner_context, components&... components)
							{
								(instance.*method)(inner_context, components...);
							},
							_context, rawComponents);
					else
						std::invoke(method);
				});
		}

		template<typename callback_fn>
		system_manager& each(callback_fn&& callback, bool context_needed)
		{
			return set_each_callback([callback = std::forward<callback_fn>(callback)](context& _context, std::vector<void*>& rawComponents)
				{
					if constexpr (sizeof...(components) > 0)
						detail::invoke_with_components<components...>(
							[&callback](context& inner_context, components&... components) {
								std::invoke(callback, inner_context, components...);
							},
							_context, rawComponents);
					else
						std::invoke(callback, _context);
				});
		}

	private:
		system_impl m_system;
		system_manager& m_manager;

		template<typename F>
		system_manager& set_each_callback(F&& callback)
		{
			m_system.set_callback(std::forward<F>(callback));
			m_manager.get_systems().push_back(std::make_unique<system_impl>(std::move(m_system)));
			return m_manager;
		}
	};
} // namespace ecs