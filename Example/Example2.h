#pragma once

#include <stdio.h>
#include "../ECS/src/flecs alike/EntityManager.hpp"
#include "../ECS/src/flecs alike/SystemManager.hpp"

struct level {};

struct Velocity { int vx, vy; };
struct Position { int x, y; };

struct Input {};

void Move(Position& p, Velocity& v)
{
	p.x += v.vx;
	p.y += v.vy;
	std::cout << p.x << " " << p.y << std::endl;
}

class Example2
{
public:
	void Start()
	{
		EntityManager em;
		ECS2::SystemManager sm(em);

		em.CreateEntity()
			.Add<Position>(0, 0)
			.Add<Input>()
			.Add<Velocity>(1, 1);

		em.CreateEntity()
			.Add<Position>(1, 1)
			.Add<Velocity>(2, 2);

		sm.AddSystem<Position, Velocity>("Move")
			.With<Position>()
			.With<Velocity>()
			.Each(Move);

		sm.AddSystem<Input>("Input")
			.Each([](Input)
				{
					std::cout << "Hello World!" << std::endl;
				});
	}
};