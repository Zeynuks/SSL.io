#pragma once

#include <vector>
#include <optional>

#include "Entity.hpp"

class EntityManager
{
public:
	EntityManager() {}

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
		m_currentScope
			? m_scopes[*m_currentScope].push_back(&entity)
			: m_scopes[m_defaultScope].push_back(&entity);

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
	}

	void Reset()
	{
		m_entities.clear();
		m_scopes.clear();
		m_scopes[m_defaultScope] = {};
		m_nextID = 0;
	}

private:
	struct initial {};

	size_t m_nextID = 0;

	const std::type_index m_defaultScope = typeid(initial);
	std::optional<std::type_index> m_currentScope;
	std::vector<std::unique_ptr<Entity>> m_entities;
	std::unordered_map<std::type_index, std::vector<Entity*>> m_scopes = { {m_defaultScope, {}} };

	Entity& CreateEntityInternal()
	{
		m_entities.emplace_back(std::make_unique<Entity>(m_nextID++));
		return *m_entities.back();
	}
};