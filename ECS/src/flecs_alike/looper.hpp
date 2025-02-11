#pragma once

#include <vector>
#include <chrono>
#include <thread>
#include <memory>
#include <optional>
#include <stdexcept>
#include <functional>

#include "system_manager.hpp"

namespace ecs
{
	class looper
	{
		using clock = std::chrono::high_resolution_clock;
		using duration = std::chrono::duration<float>;

		system_manager& m_systemManager;

	public:
		looper(system_manager& systemManager) : m_systemManager(systemManager) {}

		void frame(float delta_time)
		{
			m_systemManager.update(delta_time);
		}

		void loop(std::optional<unsigned int> targetFPS = std::nullopt)
		{
			auto frameDuration = targetFPS.has_value()
				? duration(1.0f / targetFPS.value())
				: duration(0);

			while (true)
			{
				auto startTime = clock::now();

				float delta_time = std::chrono::duration_cast<duration>(clock::now() - startTime).count();
				frame(delta_time);

				if (targetFPS.has_value())
				{
					auto elapsedTime = clock::now() - startTime;
					auto sleepDuration = frameDuration - elapsedTime;

					if (sleepDuration > std::chrono::milliseconds(0))
					{
						std::this_thread::sleep_for(sleepDuration);
					}
				}
			}
		}
	};
} // namespace ECS