#pragma once

#include "event_bus.hpp"
#include "entity_manager.hpp"

namespace ecs
{
	/**
	* @brief Класс контекста. 
	* @brief Предоставляет некоторую служебную информацию
	* @brief внутри системы
	*/
	class context
	{
	public:
		context(ecs::entity_manager& entity, ecs::event_bus& event_bus) : m_entity(entity), m_event_bus(event_bus) {}
		float delta_time = 0;
		size_t entity_id = 0;

		ecs::entity_manager& entity() { return m_entity; }
		ecs::event_bus& event_bus() { return m_event_bus; }

	private:
		ecs::entity_manager& m_entity;
		ecs::event_bus& m_event_bus;
	};
} //namespace ecs