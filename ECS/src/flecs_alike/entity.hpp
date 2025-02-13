#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

namespace ecs
{
/**
 * @brief Класс сущности
 */
class entity
{
public:
	entity(size_t ID)
		: m_ID(ID)
		, valid(true)
	{
	}

	/**
	 * @brief Добавляет компонент к сущности.
	 * @brief В параметрах указываются параметры для конструктора компонента.
	 */
	template <typename T, typename... Args>
	entity& add(Args&&... args)
	{
		m_components[typeid(T)] = std::make_shared<T>(std::forward<Args>(args)...);
		return *this;
	}

	/**
	 * @brief Возвращает указатель на указанный компонент у сущности.
	 * @brief Если не удалось найти компонент, то вернётся нулевой указатель.
	 */
	template <typename T>
	T* get()
	{
		auto it = m_components.find(typeid(T));
		return (it != m_components.end())
			? static_cast<T*>(it->second.get())
			: nullptr;
	}

	/**
	 * @brief Возвращает указатель на указанный компонент у сущности.
	 * @brief Если не удалось найти компонент, то вернётся нулевой указатель.
	 */
	void* get(std::type_index componentType)
	{
		auto it = m_components.find(componentType);
		return (it != m_components.end())
			? it->second.get()
			: nullptr;
	}

	template <typename T>
	void remove()
	{
		auto it = m_components.find(typeid(T));
		if (it != m_components.end())
		{
			m_components.erase(it);
		}
	}

	/**
	 * @brief Возвращает указатель на указанный компонент у сущности.
	 * @brief Если не удалось найти компонент, то вернётся нулевой указатель.
	 */
	template <typename T>
	bool has() const
	{
		return m_components.find(typeid(T)) != m_components.end();
	}

	/**
	 * @brief Возвращает указатель на указанный компонент у сущности.
	 * @brief Если не удалось найти компонент, то вернётся нулевой указатель.
	 */
	bool has(std::type_index component) const
	{
		return m_components.find(component) != m_components.end();
	}

	/**
	 * @brief Помечает сущность как невалидную
	 * @see ecs::entity_manager::invalidate()
	 */
	void destruct()
	{
		valid = false;
	}

	/**
	 * @brief Возвращает идентификатор сущности
	 */
	size_t ID() const { return m_ID; }

	/**
	 * @brief Проверяет является ли сущность валидной
	 * @see ecs::entity_manager::invalidate()
	 */
	bool is_valid() const { return valid; }

private:
	size_t m_ID;
	bool valid;

	std::unordered_map<std::type_index, std::shared_ptr<void>> m_components;
};
}; // namespace ecs
