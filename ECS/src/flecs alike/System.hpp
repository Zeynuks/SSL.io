#pragma once

#include <string>
#include <vector>
#include <typeindex>

#include "context.hpp"

namespace ecs
{
	class system
	{
	public:
		virtual ~system() = default;
		virtual void Execute() = 0;
	};

	class system_impl
	{
	public:
		using CallbackType = std::function<void(context&, std::vector<void*>&)>;

		system_impl(const std::string& name) : m_name(name) {}

		const std::string& get_name() const { return m_name; }
		const std::vector<std::type_index>& get_filters() const { return m_filters; }

		void set_callback(CallbackType callback) { m_callback = std::move(callback); }
		const CallbackType& get_callback() const { return m_callback; }

		void add_filter(std::type_index filter) { m_filters.push_back(filter); }

	private:
		std::string m_name;
		std::vector<std::type_index> m_filters;
		CallbackType m_callback;
	};
} //namespace ecs