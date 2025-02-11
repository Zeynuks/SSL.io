#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <type_traits>

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

	class system_storage
	{
	public:
		virtual ~system_storage() = default;
		virtual std::vector<std::unique_ptr<system_impl>>& get_systems() = 0;
	};

	/**
	* @brief Класс для конструирования системы
	*/
	template<typename... components>
	class system_builder
	{
	public:
		system_builder(system_storage& manager, const std::string& name) : m_manager(manager), m_system(name)
		{
			(m_system.add_filter(typeid(components)), ...);
		}

		/**
		* @brief Добавляет фильтр к конструируемой системе.
		*/
		template<typename T>
		system_builder& with()
		{
			m_system.add_filter(typeid(T));
			return *this;
		}

		/**
		* @brief Завершает создание системы и устанавливает функцию-обработчик.
		* @brief Функция-обработчик должна принимать аргументы в том же порядке,
		* @brief в каком они указаны в системе.
		* @see ecs::system_manager::system()
		*/
		template<typename callback_fn>
		system_storage& each(callback_fn&& callback)
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

		/**
		* @brief Завершает создание системы и устанавливает метод-обработчик.
		* @brief Метод-обработчик должен принимать аргументы в том же порядке,
		* @brief в каком они указаны в системе.
		* @brief Также требует ссылку на объект класса.
		* @see ecs::system_manager::system()
		*/
		template<typename T, typename method>
		system_storage& each(method _method, T& instance)
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

		/**
		* @brief Завершает создание системы и устанавливает метод-обработчик.
		* @brief Метод-обработчик должен принимать первым аргументом экземпляр контекста.
		* @brief Метод-обработчик должен принимать аргументы в том же порядке,
		* @brief в каком они указаны в системе.
		* @brief Также требует ссылку на объект класса.
		* @see ecs::system_manager::system()
		*/
		template<typename context_type = context, typename T, typename method>
		system_storage& each(method _method, T& instance, bool context_needed)
		{
			return set_each_callback([method = std::forward<method>(_method), &instance](context_type& _context_base, std::vector<void*>& rawComponents)
				{
					if constexpr (sizeof...(components) > 0)
						detail::invoke_with_components<components...>(
							[&method, &instance](context_type& inner_context, components&... components)
							{
								(instance.*method)(inner_context, components...);
							},
							_context_base, rawComponents);
					else
						std::invoke(method);
				});
		}

		/**
		* @brief Завершает создание системы и устанавливает функцию-обработчик.
		* @brief Функция-обработчик должна принимать первым аргументом экземпляр контекста.
		* @brief Функция-обработчик должна принимать аргументы в том же порядке,
		* @brief в каком они указаны в системе.
		* @see ecs::system_manager::system()
		*/
		template<typename callback_fn>
		system_storage& each(callback_fn&& callback, bool context_needed)
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
		system_storage& m_manager;

		template<typename F>
		system_storage& set_each_callback(F&& callback)
		{
			m_system.set_callback(std::forward<F>(callback));
			m_manager.get_systems().push_back(std::make_unique<system_impl>(std::move(m_system)));
			return m_manager;
		}
	};

	/**
	* @brief Класс для работы с коллекцией систем
	*/
	class system_manager : private system_storage
	{
	public:
		system_manager(entity_manager& em)
			: m_entityManager(em)
			, m_context(em, m_event_bus)
		{
		}

		/**
		* @brief Возвращает класс для конструирования системы.
		* @brief Важен порядок в котором передаются компоненты.
		* @see ecs::system_builder::each()
		*/
		template<typename... components>
		auto system(const std::string& name)
		{
			return system_builder<components...>(*this, name);
		}

		/**
		* @brief Возвращает шину событий
		*/
		ecs::event_bus& event_bus() { return m_event_bus; }

		/**
		* @brief Вызывает функцию-обработчик для каждой системы
		*/
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
						m_context.entity_id = entity->ID();
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
		ecs::event_bus m_event_bus;

		std::vector<std::unique_ptr<system_impl>>& get_systems() override
		{
			return m_systems;
		}
	};
} // namespace ecs