#pragma once

#include "EventBus.h"

namespace ECS
{
	class System
	{
	protected:
		EventBus* m_eventBus = nullptr;

	public:
		virtual ~System() = default;
		void SetEventBus(EventBus* eventBus)
		{
			m_eventBus = eventBus;
		}

		virtual void Execute() = 0;
		virtual void RegisterEvents() = 0;
	};
} // namespace ECS