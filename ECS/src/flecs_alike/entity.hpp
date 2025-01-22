#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

namespace ecs
{
	class entity
	{
	public:
		entity(size_t ID) : m_ID(ID), valid(true) {}

		template<typename T, typename... Args>
		entity& add(Args&&... args)
		{
			m_components[typeid(T)] = std::make_shared<T>(std::forward<Args>(args)...);
			return *this;
		}

		template<typename component>
		component* get()
		{
			auto it = m_components.find(typeid(component));
			return (it != m_components.end())
				? static_cast<component*>(it->second.get())
				: nullptr;
		}

		void* get(std::type_index componentType)
		{
			auto it = m_components.find(componentType);
			return (it != m_components.end())
				? it->second.get()
				: nullptr;
		}

		template<typename component>
		bool has() const
		{
			return m_components.find(typeid(component)) != m_components.end();
		}

		bool has(std::type_index component) const
		{
			return m_components.find(component) != m_components.end();
		}

		void destruct()
		{
			valid = false;
		}

		size_t ID() const { return m_ID; }
		bool is_valid() const { return valid; }

	private:
		size_t m_ID;
		bool valid;

		std::unordered_map<std::type_index, std::shared_ptr<void>> m_components;
	};
} //namespace ecs