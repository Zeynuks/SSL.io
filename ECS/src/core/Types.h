#pragma once
#include <bitset>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace ECS
{
	using EntityID = size_t;
	
	using ComponentID = size_t;
	
	using TypeID = std::type_index;

	constexpr unsigned int MAX_COMPONENTS = 128;
	
	using ComponentMask = std::bitset<MAX_COMPONENTS>;
	
	using Clock = std::chrono::high_resolution_clock;
	
	using Duration = std::chrono::duration<float>;
} // namespace ECS