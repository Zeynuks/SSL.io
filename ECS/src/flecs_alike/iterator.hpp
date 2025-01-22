#pragma once
#include <cstddef>
#include "entity.hpp"

namespace ecs
{
	class entity_iterator
	{
	private:
		const std::vector<entity*>& m_entities;
		size_t m_index;

	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = entity*;
		using difference_type = std::ptrdiff_t;
		using pointer = entity**;
		using reference = entity&;

		entity_iterator(const std::vector<entity*>& entities, size_t index = 0) : m_entities(entities), m_index(index) {}

		entity& operator*() const { return *m_entities[m_index]; }
		entity* operator->() const { return m_entities[m_index]; }
		size_t GetIndex() const { return m_index; }

		entity_iterator& operator++()
		{
			++m_index;
			return *this;
		}

		entity_iterator operator++(int)
		{
			entity_iterator tmp = *this;
			++(*this);
			return tmp;
		}

		entity_iterator& operator--()
		{
			--m_index;
			return *this;
		}

		entity_iterator operator--(int)
		{
			entity_iterator tmp = *this;
			--(*this);
			return tmp;
		}

		entity_iterator& operator+=(std::ptrdiff_t n)
		{
			m_index += n;
			return *this;
		}

		entity_iterator& operator-=(std::ptrdiff_t n)
		{
			m_index -= n;
			return *this;
		}

		entity_iterator operator+(std::ptrdiff_t n) const
		{
			entity_iterator tmp = *this;
			tmp.m_index = m_index + n;
			return tmp;
		}

		entity_iterator operator-(std::ptrdiff_t n) const
		{
			entity_iterator tmp = *this;
			tmp.m_index = m_index - n;
			return tmp;
		}

		std::ptrdiff_t operator-(const entity_iterator& other) const
		{
			return static_cast<std::ptrdiff_t>(m_index) - static_cast<std::ptrdiff_t>(other.m_index);
		}

		bool operator==(const entity_iterator& other) const
		{
			return m_index == other.m_index;
		}

		bool operator!=(const entity_iterator& other) const
		{
			return m_index != other.m_index;
		}

		bool operator<(const entity_iterator& other) const { return m_index < other.m_index; }
		bool operator>(const entity_iterator& other) const { return m_index > other.m_index; }
		bool operator<=(const entity_iterator& other) const { return m_index <= other.m_index; }
		bool operator>=(const entity_iterator& other) const { return m_index >= other.m_index; }

		entity& operator[](std::ptrdiff_t n) const
		{
			return *(*this + n);
		}
	};
} //namespace ecs