#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

class Entity
{
public:
	Entity(size_t ID) : m_ID(ID), valid(true) {}

	template<typename T, typename... Args>
	Entity& Add(Args&&... args)
	{
		m_components[typeid(T)] = std::make_shared<T>(std::forward<Args>(args)...);
		return *this;
	}

	template<typename Component>
	Component* Get()
	{
		auto it = m_components.find(typeid(Component));
		return (it != m_components.end())
			? static_cast<Component*>(it->second.get())
			: nullptr;
	}

	template<typename Component>
	bool Has() const
	{
		return m_components.find(typeid(Component)) != m_components.end();
	}

	bool Has(std::type_index component) const
	{
		return m_components.find(component) != m_components.end();
	}

	void Destruct()
	{
		valid = false;
	}

	size_t GetID() const { return m_ID; }
	bool IsValid() const { return valid; }

private:
	size_t m_ID;
	bool valid;

	std::unordered_map<std::type_index, std::shared_ptr<void>> m_components;
};