#pragma once

#include "EventBus.hpp"

namespace ECS
{
	class System
	{
	protected:
		EventBus* m_eventBus = nullptr;

	public:
		virtual ~System() = default;
		void SetEventBus(EventBus* bus)
		{
			m_eventBus = bus;
		}

		virtual void Execute() = 0;
		virtual void RegisterEvents() = 0;
	};
} // namespace ECS