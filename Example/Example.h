#pragma once

#include <conio.h>
#include <iostream>
#include "../ECS/Engine.hpp"

struct Transform { int x = 0, y = 0; };

struct InputComponent {};
struct Renderable {};
struct Moveable { int dx = 0, dy = 0; };

struct KeyPressed : ECS::Event {
	char key;
	explicit KeyPressed(char key) : key(key) {}
};

struct MovementEvent : ECS::Event {
	ECS::EntityID entity;
	int dx, dy;
	explicit MovementEvent(ECS::EntityID entity, int dx, int dy) : entity(entity), dx(dx), dy(dy) {}
};

class InputSystem : public ECS::System {
	ECS::ECSManager* m_manager = nullptr;

public:
	InputSystem(ECS::ECSManager* em) : m_manager(em) {}

	void RegisterEvents() override {}

	void Execute() override
	{
		for (auto entity : m_manager->GetEntitiesWith<InputComponent>()) {
			char input = GetInput();
			if (input == '\0') return;
			int dx = 0, dy = 0;
			switch (input) {
			case 'w':
			case 'W': dy = -1; break;
			case 's':
			case 'S': dy = 1; break;
			case 'a':
			case 'A': dx = -1; break;
			case 'd':
			case 'D': dx = 1; break;
			}
			m_eventBus->Publish(MovementEvent(entity, dx, dy));
		}
	}

	char GetInput() { return _kbhit() ? _getch() : '\0'; }
};

class MovementSystem : public ECS::System {
	ECS::ECSManager* m_manager = nullptr;

public:
	MovementSystem(ECS::ECSManager* em) : m_manager(em) {}

	void RegisterEvents() override
	{
		if (m_eventBus)
		{
			m_eventBus->Subscribe<MovementEvent>([this](const MovementEvent& event) {
				auto transform = m_manager->GetComponent<Transform>(event.entity);
				transform->x += event.dx;
				transform->y += event.dy;
				std::cout << "MovementSystem: Entity " << event.entity << " moved to ("
					<< transform->x << ", " << transform->y << ")\n";
				});
		}
	}

	void Execute() override {}
};

class RenderSystem : public ECS::System {
	ECS::ECSManager* m_manager = nullptr;

public:
	RenderSystem(ECS::ECSManager* em) : m_manager(em) {}

	void RegisterEvents() override {}

	void Execute() override
	{
		for (auto entity : m_manager->GetEntitiesWith<Renderable, Transform>())
		{
			const auto transform = m_manager->GetComponent<Transform>(entity);
			std::cout << "RenderSystem: Rendering entity " << entity << " at ("
				<< transform->x << ", " << transform->y << ")\n";
		}
	}
};

class Example
{
public:
	void Start()
	{
		ECS::ECSManager em;
		auto eventBus = std::make_shared<ECS::EventBus>();
		auto sm = std::make_shared<ECS::SystemManager>(eventBus);
		ECS::Looper gameLoop(sm);

		em.RegisterComponent<Moveable>();
		em.RegisterComponent<Transform>();
		em.RegisterComponent<Renderable>();
		em.RegisterComponent<InputComponent>();

		auto player = em.CreateEntity();
		em.AddComponent(player, Transform());
		em.AddComponent(player, Moveable());
		em.AddComponent(player, Renderable());
		em.AddComponent(player, InputComponent());

		sm->AddSystem<InputSystem>(&em);
		sm->AddSystem<MovementSystem>(&em);
		sm->AddSystem<RenderSystem>(&em);

		std::cout << "Running single update cycle:\n";
		gameLoop.RunFrame(1.f / 60);

		std::cout << "\nStarting game loop with 2 FPS:\n";
		gameLoop.RunLoop(2);
	}
};