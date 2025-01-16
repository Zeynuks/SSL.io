#pragma once


#include <stdexcept>
#include "Types.h"
#include "ComponentArray.h"

namespace ECS
{
	class ComponentManager
	{
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
	};
} //namespace ECS