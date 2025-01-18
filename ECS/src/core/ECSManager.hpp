#pragma once

#include <queue>
#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "Types.h"
#include "ComponentArray.hpp"

/// TODO: декомпозировать класс
namespace ECS
{
	class ECSManager
	{
		// Entity management
		std::vector<EntityID> m_entities;
		std::unordered_map<EntityID, ComponentMask> m_entitiesSignatures;
		EntityID m_nextEntityId = 0;

		// Component management
		std::unordered_map<TypeID, ComponentID> m_componentTypes;
		std::unordered_map<ComponentID, std::unique_ptr<IComponentArray>> m_componentArrays;

		template<typename ComponentType>
		ComponentArray<ComponentType>* GetArray(ComponentID id)
		{
			auto it = m_componentArrays.find(id);
			return (it != m_componentArrays.end() && it->second)
				? static_cast<ComponentArray<ComponentType>*>(it->second.get())
				: nullptr;
		}

		template<typename ComponentType>
		void CreateArray(TypeID typeIndex)
		{
			ComponentID id = m_componentTypes.size();
			m_componentTypes[typeIndex] = id;
			m_componentArrays[id] = std::make_unique<ComponentArray<ComponentType>>();
		}

	public:
		// Entity methods
		EntityID CreateEntity()
		{
			EntityID entity = m_nextEntityId++;
			m_entities.push_back(entity);
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
			for (const auto& id : m_entities)
			{
				auto it = std::remove_if(m_entities.begin(), m_entities.end(),
					[id](const EntityID& entity) {return entity == id; });

				if (it != m_entities.end())
				{
					m_entities.erase(it, m_entities.end());
				}
			}
		}

		// Component methods
		template<typename ComponentType>
		void RegisterComponent()
		{
			TypeID typeIndex = std::type_index(typeid(ComponentType));
			if (m_componentTypes.find(typeIndex) != m_componentTypes.end())
			{
				throw std::runtime_error("Component is already registered.");
			}
			CreateArray<ComponentType>(typeIndex);
		}

		template<typename ComponentType>
		void AddComponent(EntityID entity, ComponentType component)
		{
			TypeID typeIndex = std::type_index(typeid(ComponentType));
			if (m_componentTypes.find(typeIndex) == m_componentTypes.end())
			{
				throw std::runtime_error("Component is not registered.");
			}
			ComponentID id = m_componentTypes[typeIndex];
			auto componentArray = GetArray<ComponentType>(id);
			if (!componentArray) {
				throw std::runtime_error("Component array is not initialized.");
			}
			componentArray->Add(entity, component);

			ComponentMask& entityMask = m_entitiesSignatures[entity];
			entityMask.set(id);
		}

		template<typename ComponentType>
		void RemoveComponent(EntityID entity)
		{
			TypeID typeIndex = std::type_index(typeid(ComponentType));
			if (m_componentTypes.find(typeIndex) == m_componentTypes.end())
			{
				throw std::runtime_error("Component is not registered.");
			}
			ComponentID id = m_componentTypes[typeIndex];
			auto componentArray = GetArray<ComponentType>(id);
			if (!componentArray) {
				throw std::runtime_error("Component array is not initialized.");
			}
			componentArray->Remove(entity);

			ComponentMask& entityMask = m_entitiesSignatures[entity];
			entityMask.reset(id);
		}

		template<typename ComponentType>
		ComponentType* GetComponent(EntityID entity)
		{
			TypeID typeIndex = std::type_index(typeid(ComponentType));
			if (m_componentTypes.find(typeIndex) == m_componentTypes.end())
			{
				throw std::runtime_error("Component is not registered.");
			}
			ComponentID id = m_componentTypes[typeIndex];
			auto componentArray = GetArray<ComponentType>(id);
			if (!componentArray) {
				throw std::runtime_error("Component array is not initialized.");
			}
			return componentArray->Get(entity);
		}

		template<typename... Components>
		std::vector<EntityID> GetEntitiesWith()
		{
			ComponentMask requiredMask;
			(requiredMask.set(m_componentTypes[std::type_index(typeid(Components))]), ...);

			std::vector<EntityID> matchingEntities;
			for (const auto& [entity, signature] : m_entitiesSignatures)
			{
				if ((signature & requiredMask) == requiredMask)
				{
					matchingEntities.push_back(entity);
				}
			}

			return matchingEntities;
		}
	};
} //namespace ECS