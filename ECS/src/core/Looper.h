#pragma once

#include <vector>
#include <chrono>
#include <thread>
#include <memory>
#include <optional>
#include <stdexcept>
#include <functional>

#include "Types.h"
#include "SystemManager.h"

namespace ECS
{
	class Looper
	{
		using SystemManagersVector = std::vector<std::shared_ptr<SystemManager>>;

		SystemManagersVector m_systemManagers;

	public:
		Looper(std::shared_ptr<SystemManager> systemManager)
		{
			if (!systemManager)
			{
				throw std::invalid_argument("Passing argument is nullptr. Can not create a Looper object without a single SystemManager");
			}

			m_systemManagers = { std::move(systemManager) };
		}

		Looper(SystemManagersVector systemManagers)
		{
			if (systemManagers.empty())
			{
				throw std::invalid_argument("Passing argument is an empty vector. Can not create a Looper object without a single SystemManager");
			}

			m_systemManagers = systemManagers;
		}

		void RunFrame(float deltaTime)
		{
			for (const auto& systemManager : m_systemManagers)
			{
				systemManager->ExecuteAll();
			}
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