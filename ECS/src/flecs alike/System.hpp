#pragma once

#include <string>
#include <vector>
#include <typeindex>

#include "EntityManager.hpp"

namespace ECS2
{
	class ISystem
	{
	public:
		virtual ~ISystem() = default;
		virtual void Execute() = 0;
	};

    class System 
	{
    public:
        using CallbackType = std::function<void(std::vector<void*>&)>;

        System(const std::string& name) : m_name(name) {}

        const std::string& GetName() const { return m_name; }
        const std::vector<std::type_index>& GetFilters() const { return m_filters; }

        void SetCallback(CallbackType callback) { m_callback = std::move(callback); }
        const CallbackType& GetCallback() const { return m_callback; }

        void AddFilter(std::type_index filter) { m_filters.push_back(filter); }

		bool HasFilters(Entity& entity)
		{
			for (const auto& filter : m_filters) {
				if (entity.Has(filter))
				{
					continue;
				}
				return false;
			}
			return true;
		}

    private:
        std::string m_name;
        std::vector<std::type_index> m_filters;
        CallbackType m_callback;
    };
} //namespace ECS2