#pragma once

#include <vector>
#include <chrono>
#include <thread>
#include <memory>
#include <optional>
#include <stdexcept>
#include <functional>

#include "SystemManager.hpp"

namespace ECS2
{
	class Looper
	{
		using Clock = std::chrono::high_resolution_clock;
		using Duration = std::chrono::duration<float>;

		std::shared_ptr<SystemManager> m_systemManager;

	public:
		Looper(std::shared_ptr<SystemManager> systemManager)
		{
			if (!systemManager)
			{
				throw std::invalid_argument("Passing argument is nullptr. Looper requires at least one SystemManager");
			}

			m_systemManager = std::move(systemManager);
		}

		void RunFrame(float deltaTime)
		{
			m_systemManager->Update(deltaTime);
		}

		void RunLoop(std::optional<unsigned int> targetFPS = std::nullopt)
		{
			auto frameDuration = targetFPS.has_value()
				? Duration(1.0f / targetFPS.value())
				: Duration(0);

			while (true)
			{
				auto startTime = Clock::now();

				float deltaTime = std::chrono::duration_cast<Duration>(Clock::now() - startTime).count();
				RunFrame(deltaTime);

				if (targetFPS.has_value())
				{
					auto elapsedTime = Clock::now() - startTime;
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