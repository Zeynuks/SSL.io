#pragma once

#include "../ECS/src/flecs alike/EntityManager.hpp"
#include "../ECS/src/flecs alike/SystemManager.hpp"
#include "../ECS/src/flecs alike/Looper.hpp"

#include "SFML/Graphics.hpp"

//scopes
struct render {};

//components
struct Velocity { float vx, vy; };
struct Position { float x, y; };
struct Renderable
{
	Renderable(sf::Color color) : rect({ 50, 50 })
	{
		rect.setFillColor(color);
	}
	sf::RectangleShape rect;
};
struct Input
{
	bool moveLeft = false;
	bool moveRight = false;
	bool moveUp = false;
	bool moveDown = false;
};
struct Camera { sf::View& camera; sf::RenderWindow& window; };
struct Window { sf::RenderWindow& window; };

//systems
void Draw(ECS2::Context& ctx, Window& w)
{
	for (auto& entity : ctx.entity().AtScope<render>().GetAll())
	{
		auto pos = entity->Get<Position>();
		auto rect = entity->Get<Renderable>();
		if (pos && rect)
		{
			rect->rect.setPosition(pos->x, pos->y);
			w.window.draw(rect->rect);
		}
	}
}

void Move(ECS2::Context ctx, Position& p, Velocity& v)
{
	p.x += v.vx * ctx.deltaTime;
	p.y += v.vy * ctx.deltaTime;
}

void HandleInput(Input& input, Velocity& v)
{
	v.vx = 0.f;
	v.vy = 0.f;

	if (input.moveLeft) v.vx -= 500.f;
	if (input.moveRight) v.vx += 500.f;
	if (input.moveUp) v.vy -= 500.f;
	if (input.moveDown) v.vy += 500.f;
}

void MoveCamera(Camera& c, Position& p)
{
	auto pos = sf::Vector2f(p.x, p.y);
	auto& camera = c.camera;
	camera.setCenter(camera.getCenter() + (pos - camera.getCenter()) * 0.1f);
	c.window.setView(camera);
}

//game class
class Example2
{
public:
	void Start()
	{
		ECS2::EntityManager em;
		auto sm = std::make_shared<ECS2::SystemManager>(em);
		ECS2::Looper looper(sm);

		sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "ECS Example");
		sf::View camera = window.getView();
		sf::Event event{};
		sf::Clock clock;

		auto& playerEntity = em.AtScope<render>()
			.CreateEntity()
			.Add<Velocity>(0.f, 0.f)
			.Add<Position>(0.f, 0.f)
			.Add<Renderable>(sf::Color::Green)
			.Add<Camera>(camera, window)
			.Add<Input>();

		em.CreateEntity()
			.Add<Window>(window);

		em.AtScope<render>()
			.CreateEntity()
			.Add<Position>(100.f, 100.f)
			.Add<Renderable>(sf::Color::Red);

		sm->AddSystem<Window>("Draw")
			.Each(Draw, true);

		sm->AddSystem<Position, Velocity>("Move")
			.Each(Move, true);

		sm->AddSystem<Input, Velocity>("HandleInput")
			.Each(HandleInput);

		sm->AddSystem<Camera, Position>("MoveCamera")
			.Each(MoveCamera);

		while (window.isOpen())
		{
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed) window.close();
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::A) playerEntity.Get<Input>()->moveLeft = true;
					if (event.key.code == sf::Keyboard::D) playerEntity.Get<Input>()->moveRight = true;
					if (event.key.code == sf::Keyboard::W) playerEntity.Get<Input>()->moveUp = true;
					if (event.key.code == sf::Keyboard::S) playerEntity.Get<Input>()->moveDown = true;
				}
				if (event.type == sf::Event::KeyReleased)
				{
					if (event.key.code == sf::Keyboard::A) playerEntity.Get<Input>()->moveLeft = false;
					if (event.key.code == sf::Keyboard::D) playerEntity.Get<Input>()->moveRight = false;
					if (event.key.code == sf::Keyboard::W) playerEntity.Get<Input>()->moveUp = false;
					if (event.key.code == sf::Keyboard::S) playerEntity.Get<Input>()->moveDown = false;
				}
			}

			float dt = clock.restart().asSeconds();
			window.clear();
			looper.RunFrame(dt);
			window.display();
		}
	}
};