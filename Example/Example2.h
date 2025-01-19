#pragma once

#include "../ECS/src/flecs alike/EntityManager.hpp"
#include "../ECS/src/flecs alike/SystemManager.hpp"

struct level {};

struct Velocity { int vx, vy; };
struct Position { int x, y; };

struct Input {};

void Move(ECS2::Context ctx, Position& p, Velocity& v)
{
	p.x += v.vx;
	p.y += v.vy;
	std::cout << "Entity " << ctx.entityID << " position " << p.x << " " << p.y << std::endl;
}

class StatelessUserSystem
{
public:
	static void Execute(Input)
	{
		std::cout << "Stateless system executed" << std::endl;
	}
};

class StateUserSystem
{
public:
	void Execute()
	{
		std::cout << "State system was executed " << m_count++ << " times" << std::endl;
	}

private:
	size_t m_count = 0;
};

class Example2
{
public:
	void Start()
	{
		ECS2::EntityManager em;
		ECS2::SystemManager sm(em);
		StateUserSystem sus;

		em.CreateEntity()
			.Add<Position>(0, 0)
			.Add<Velocity>(1, 1);

		em.CreateEntity()
			.Add<Position>(1, 1)
			.Add<Input>()
			.Add<Velocity>(2, 2);

		sm.AddSystem<Position, Velocity>("Move")
			.Each(Move, true);

		sm.AddSystem<Input>("Stateless")
			.Each(StatelessUserSystem::Execute);

		sm.AddSystem("State")
			.Each([&sus]()
				{
					std::cout << "State system is about to be executed" << std::endl;
					sus.Execute();
				});

		while (true)
		{
			sm.Update();
		}
	}
};