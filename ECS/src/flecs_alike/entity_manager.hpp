#pragma once

#include <vector>
#include <optional>

#include "entity.hpp"
#include "iterator.hpp"

namespace ecs
{
	class entity_manager
	{
	public:
		template<typename scope>
		entity_manager& at_scope()
		{
			m_currentScope = typeid(scope);
			return *this;
		}

		std::vector<entity*>& all()
		{
			return (m_currentScope)
				? m_scopes[*m_currentScope]
				: m_scopes[m_defaultScope];
		}

		entity& create()
		{
			auto& entity = create_internal();
			(m_currentScope)
				? m_scopes[*m_currentScope].push_back(&entity)
				: m_scopes[m_defaultScope].push_back(&entity);
			update_all();
			m_currentScope.reset();
			return entity;
		}

		void invalidate()
		{
			auto& entities = all();
			entities.erase(
				std::remove_if(entities.begin(), entities.end(),
					[](entity* e) { return !e->is_valid(); }),
				entities.end());
			update_all();
			m_currentScope.reset();
		}

		std::pair<entity_iterator, entity_iterator> get_all_entities()
		{
			return { entity_iterator(m_allEntities), entity_iterator(m_allEntities, m_allEntities.size()) };
		}

		void reset()
		{
			m_entities.clear();
			m_scopes.clear();
			m_scopes[m_defaultScope] = {};
			m_allEntities.clear();
			m_currentScope.reset();
			m_nextID = 0;
		}

	private:
		struct initial {};

		size_t m_nextID = 0;

		const std::type_index m_defaultScope = typeid(initial);
		std::optional<std::type_index> m_currentScope;
		std::vector<std::unique_ptr<entity>> m_entities;
		std::vector<entity*> m_allEntities;
		std::unordered_map<std::type_index, std::vector<entity*>> m_scopes = { {m_defaultScope, {}} };

		entity& create_internal()
		{
			m_entities.emplace_back(std::make_unique<entity>(m_nextID++));
			return *m_entities.back();
		}

		void update_all()
		{
			m_allEntities.clear();
			for (const auto& [_, entities] : m_scopes) {
				m_allEntities.insert(m_allEntities.end(), entities.begin(), entities.end());
			}
		}
	};
} //namespace ecs