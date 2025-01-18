#pragma once

#include <unordered_map>

#include "Types.h"

using namespace ECS;

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void Remove(EntityID entity) = 0;
	virtual bool Has(EntityID entity) const = 0;
};

template<typename ComponentType>
class ComponentArray : public IComponentArray
{
	std::unordered_map<EntityID, ComponentType> m_componentData;

public:
	void Add(EntityID entity, const ComponentType& component)
	{
		m_componentData[entity] = component;
	}

	void Remove(EntityID entity) override
	{
		m_componentData.erase(entity);
	}

	bool Has(EntityID entity) const override
	{
		return m_componentData.find(entity) != m_componentData.end();
	}

	ComponentType* Get(EntityID entity)
	{
		auto it = m_componentData.find(entity);
		return it != m_componentData.end() ? &it->second : nullptr;
	}
};