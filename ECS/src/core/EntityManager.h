#pragma once

#include <queue>
#include <stdexcept>

namespace ECS
{
	class EntityManager
	{
		std::queue<EntityID> m_availableEntities;
		std::unordered_map<EntityID, ComponentMask> m_entitiesSignatures;
		EntityID m_nextEntityId = 0;

	public:
		EntityID CreateEntity()
		{
			EntityID entity;
			if (m_availableEntities.empty())
			{
				entity = m_nextEntityId++;
			}
			else
			{
				entity = m_availableEntities.front();
				m_availableEntities.pop();
			}

			m_entitiesSignatures[entity] = ComponentMask();
			return entity;
		}

		void DestroyEntity(EntityID entity)
		{
			if (m_entitiesSignatures.find(entity) == m_entitiesSignatures.end())
			{
				throw std::runtime_error("Attempted to destroy non-existent entity.");
			}

			m_entitiesSignatures.erase(entity);
			m_availableEntities.push(entity);
		}

		void SetSignature(EntityID entity, ComponentMask signature)
		{
			if (m_entitiesSignatures.find(entity) == m_entitiesSignatures.end())
			{
				throw std::runtime_error("Attempted to set a signature for a non-existent entity.");
			}

			m_entitiesSignatures[entity] = signature;
		}

		const ComponentMask& GetSignature(EntityID entity) const
		{
			auto it = m_entitiesSignatures.find(entity);
			if (it == m_entitiesSignatures.end())
			{
				throw std::runtime_error("Attempted to get a signature for a non-existent entity.");
			}

			return it->second;
		}
	};
} //namespace ECS