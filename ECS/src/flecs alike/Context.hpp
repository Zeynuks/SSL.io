#pragma once

#include "entity_manager.hpp"

namespace ecs
{
	struct context
	{
		context(entity_manager& entity) : m_entity(entity) {}

		size_t index = 0;
		float delta_time = 0;
		size_t ID = 0;

		entity_manager& entity() { return m_entity; };

	private:
		entity_manager& m_entity;
	};
} //namespace ecs