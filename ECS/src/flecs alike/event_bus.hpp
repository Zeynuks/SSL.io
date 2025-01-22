#pragma once

#include <mutex>
#include <memory>
#include <thread>
#include <vector>
#include <typeindex>
#include <functional>
#include <unordered_map>

namespace ECS
{
	struct Event
	{
		virtual ~Event() = default;
	};

	class EventBus
	{
		using EventID = std::type_index;
		using HandlerType = std::function<void(const Event&)>;
		using EventSubscribers = std::unordered_map<EventID, std::vector<HandlerType>>;

		EventSubscribers m_subscribers;
		std::mutex m_mutex;

	public:
		template <typename EventType>
		void Subscribe(std::function<void(const EventType&)> callback)
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			EventID ID = std::type_index(typeid(EventType));
			m_subscribers[ID].push_back(
				[callback](const Event& e) { callback(static_cast<const EventType&>(e)); }
			);
		}

		template<typename EventType>
		void Publish(const EventType& event)
		{
			std::vector<HandlerType> handlers;
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				EventID ID = std::type_index(typeid(EventType));
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