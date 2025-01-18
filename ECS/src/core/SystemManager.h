#pragma once

#include <memory>
#include <vector>

#include "System.h"

namespace ECS
{
	class SystemManager
	{
		std::shared_ptr<EventBus> m_eventBus = nullptr;
		std::vector<std::shared_ptr<System>> m_systems;

	public:
		explicit SystemManager(std::shared_ptr<EventBus> bus = nullptr)
			: m_eventBus(bus ? bus : std::make_shared<EventBus>()) {
		}

		template<typename SystemType, typename... Args>
		std::shared_ptr<SystemType> AddSystem(Args&&... args)
		{
			auto system = std::make_shared<SystemType>(std::forward<Args>(args)...);
			system->SetEventBus(m_eventBus.get());
			system->RegisterEvents();
			m_systems.push_back(system);
			return system;
		}

		void ExecuteAll()
		{
			for (const auto& system : m_systems)
			{
				system->Execute();
			}
		}

		EventBus& GetEventBus()
		{
			return *m_eventBus;
		}
	};
} // namespace ECS