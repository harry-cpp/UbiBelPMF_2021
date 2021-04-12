#include "physics.h"

#include "core/engine.h"
#include "core/game/transforms.h"
#include "../components/movable.h"
#include "core/graphics/sprite.h"

using namespace dagger;
using namespace brawler;

void PhysicsSystem::Run()
{
	auto objects = Engine::Registry().view<Transform, Movable, Sprite>();
	for (auto obj : objects)
	{
		auto& t = objects.get<Transform>(obj);
		auto& m = objects.get<Movable>(obj);
		auto& s = objects.get<Sprite>(obj);


		t.position.x += (m.speed.x + m.envSpeed.x) * Engine::DeltaTime();
		t.position.y += (m.speed.y - 20.0f - m.fallSpeed) * Engine::DeltaTime();

		if (t.position.y < 0)
		{
			t.position.y = 0;
			m.isOnGround = true;
			m.envSpeed.y = 0;
			m.fallSpeed = 0;
		}
		else
		{
			m.fallSpeed += 0.1;
			m.isOnGround = false;
		}

		m.prevPosition = t.position;
		m.prevSpeed = m.speed;
		m.wasOnGround = m.isOnGround;
		m.speed.x = 0;
		m.speed.y = 0;
		
		m.envSpeed *= 0.95;	
	}
}
