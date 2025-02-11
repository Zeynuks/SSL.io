#pragma once

#include <mutex>
#include <memory>
#include <thread>
#include <vector>
#include <typeindex>
#include <functional>
#include <unordered_map>

namespace ecs
{
	/**
	* @brief Базовое событие.
	* @brief Все пользовательские события
	* @brief должны наследоваться от этого класса.
	*/
	struct event
	{
		virtual ~event() = default;
	};

	/**
	* @brief Класс шины событий
	*/
	class event_bus
	{
		using event_id = std::type_index;
		using handler = std::function<void(const event&)>;
		using subscribers = std::unordered_map<event_id, std::vector<handler>>;

		subscribers m_subscribers;
		std::mutex m_mutex;

	public:
		/**
		* @brief Добавляет функцию-обработчик для события.
		* @brief Функция-обработчик будет вызвана при отправке указанного события.
		* @see ecs::event_bus::publish()
		*/
		template <typename EventType>
		void subscribe(std::function<void(const EventType&)> callback)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			event_id ID = std::type_index(typeid(EventType));
			m_subscribers[ID].push_back(
				[callback](const event& e) { callback(static_cast<const EventType&>(e)); }
			);
		}

		/**
		* @brief Отправляет переданное событие всем его обработчикам.
		* @see ecs::event_bus::publish()
		*/
		template<typename EventType>
		void publish(const EventType& event)
		{
			std::vector<handler> handlers;
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				event_id ID = std::type_index(typeid(EventType));
				auto it = m_subscribers.find(ID);
				if (it != m_subscribers.end())
				{
					handlers = it->second;
				}
			}

			std::vector<std::thread> threads;
			for (const auto& handler : handlers)
			{
				threads.emplace_back([handler, &event]() { handler(event); });
			}

			for (auto& thread : threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
		}
	};
} // namespace ECS