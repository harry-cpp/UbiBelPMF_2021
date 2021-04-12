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

		t.position.x += m.speed.x * Engine::DeltaTime();
		t.position.y += (m.speed.y - 20.0f) * Engine::DeltaTime();
		

		if (t.position.y < 0)
		{
			t.position.y = 0;
			m.isOnGround = true;
		}
		else
		{
			m.isOnGround = false;
		}

		s.position = t.position;

		m.prevPosition = t.position;
		m.prevSpeed = m.speed;
		m.wasOnGround = m.isOnGround;
		m.speed.x = 0;
		m.speed.y = 0;
	}
}
