#pragma once

#include <memory>
#include <vector>

#include "System.hpp"

namespace ECS2
{
	class ISystemManager
	{
	public:
		virtual ~ISystemManager() = default;
		virtual std::vector<std::unique_ptr<System>>& GetSystems() = 0;
	};
}