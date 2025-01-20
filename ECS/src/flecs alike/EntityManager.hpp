#pragma once

#include <vector>
#include <optional>

#include "Entity.hpp"
#include "EntityIterator.hpp"

namespace ECS2
{
	class EntityManager
	{
	public:
		template<typename ScopeType>
		EntityManager& AtScope()
		{
			m_currentScope = typeid(ScopeType);
			return *this;
		}

		std::vector<Entity*>& GetAll()
		{
			return (m_currentScope)
				? m_scopes[*m_currentScope]
				: m_scopes[m_defaultScope];
		}

		Entity& CreateEntity()
		{
			auto& entity = CreateEntityInternal();
			(m_currentScope)
				? m_scopes[*m_currentScope].push_back(&entity)
				: m_scopes[m_defaultScope].push_back(&entity);
			UpdateAllEntities();
			m_currentScope.reset();
			return entity;
		}

		void RemoveInvalid()
		{
			auto& entities = GetAll();
			entities.erase(
				std::remove_if(entities.begin(), entities.end(),
					[](Entity* e) { return !e->IsValid(); }),
				entities.end());
			UpdateAllEntities();
			m_currentScope.reset();
		}

		std::pair<EntityIterator, EntityIterator> GetAllEntities()
		{
			return { EntityIterator(m_allEntities), EntityIterator(m_allEntities, m_allEntities.size()) };
		}

		void Reset()
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
		std::vector<std::unique_ptr<Entity>> m_entities;
		std::vector<Entity*> m_allEntities;
		std::unordered_map<std::type_index, std::vector<Entity*>> m_scopes = { {m_defaultScope, {}} };

		Entity& CreateEntityInternal()
		{
			m_entities.emplace_back(std::make_unique<Entity>(m_nextID++));
			return *m_entities.back();
		}

		void UpdateAllEntities()
		{
			m_allEntities.clear();
			for (const auto& [_, entities] : m_scopes) {
				m_allEntities.insert(m_allEntities.end(), entities.begin(), entities.end());
			}
		}
	};
} //namespace ECS2