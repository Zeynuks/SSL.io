#pragma once
#include <cstddef>
#include "Entity.hpp"

namespace ECS2
{
	class EntityIterator
	{
	private:
		const std::vector<Entity*>& m_entities;
		size_t m_index;

	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = Entity*;
		using difference_type = std::ptrdiff_t;
		using pointer = Entity**;
		using reference = Entity&;

		EntityIterator(const std::vector<Entity*>& entities, size_t index = 0) : m_entities(entities), m_index(index) {}

		Entity& operator*() const { return *m_entities[m_index]; }
		Entity* operator->() const { return m_entities[m_index]; }
		size_t GetIndex() const { return m_index; }

		EntityIterator& operator++()
		{
			++m_index;
			return *this;
		}

		EntityIterator operator++(int)
		{
			EntityIterator tmp = *this;
			++(*this);
			return tmp;
		}

		EntityIterator& operator--()
		{
			--m_index;
			return *this;
		}

		EntityIterator operator--(int)
		{
			EntityIterator tmp = *this;
			--(*this);
			return tmp;
		}

		EntityIterator& operator+=(std::ptrdiff_t n)
		{
			m_index += n;
			return *this;
		}

		EntityIterator& operator-=(std::ptrdiff_t n)
		{
			m_index -= n;
			return *this;
		}

		EntityIterator operator+(std::ptrdiff_t n) const
		{
			EntityIterator tmp = *this;
			tmp.m_index = m_index + n;
			return tmp;
		}

		EntityIterator operator-(std::ptrdiff_t n) const
		{
			EntityIterator tmp = *this;
			tmp.m_index = m_index - n;
			return tmp;
		}

		std::ptrdiff_t operator-(const EntityIterator& other) const
		{
			return static_cast<std::ptrdiff_t>(m_index) - static_cast<std::ptrdiff_t>(other.m_index);
		}

		bool operator==(const EntityIterator& other) const
		{
			return m_index == other.m_index;
		}

		bool operator!=(const EntityIterator& other) const
		{
			return m_index != other.m_index;
		}

		bool operator<(const EntityIterator& other) const { return m_index < other.m_index; }
		bool operator>(const EntityIterator& other) const { return m_index > other.m_index; }
		bool operator<=(const EntityIterator& other) const { return m_index <= other.m_index; }
		bool operator>=(const EntityIterator& other) const { return m_index >= other.m_index; }

		Entity& operator[](std::ptrdiff_t n) const
		{
			return *(*this + n);
		}
	};
} //namespace ECS2