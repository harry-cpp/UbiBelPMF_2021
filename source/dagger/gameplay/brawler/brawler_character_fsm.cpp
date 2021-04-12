#include "brawler_character_fsm.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "gameplay/brawler/brawler_character_controller.h"
#include "gameplay/brawler/components/movable.h"

using namespace dagger;
using namespace brawler;

// Idle

void BrawlerCharacterFSM::Idle::Enter(BrawlerCharacterFSM::StateComponent& state_)
{
	auto& animator = Engine::Registry().get<Animator>(state_.entity);
	AnimatorPlay(animator, "souls_like_knight_character:IDLE");
}

DEFAULT_EXIT(BrawlerCharacterFSM, Idle);

void BrawlerCharacterFSM::Idle::Run(BrawlerCharacterFSM::StateComponent& state_)
{
	auto&& [sprite, input, player, transform, movable] = Engine::Registry().get<Sprite, InputReceiver, Player, Transform, Movable>(state_.entity);

	if (!movable.isOnGround || EPSILON_NOT_ZERO(input.Get("jump")))
	{
		Logger::info(movable.isOnGround);
		Logger::info(input.Get("jump"));
		GoTo(ECharacterStates::Jumping, state_);
	}
	else if (EPSILON_NOT_ZERO(input.Get("run")))
	{
		GoTo(ECharacterStates::Running, state_);
	}
}

// Running

void BrawlerCharacterFSM::Running::Enter(BrawlerCharacterFSM::StateComponent& state_)
{
	auto& animator = Engine::Registry().get<Animator>(state_.entity);
	AnimatorPlay(animator, "souls_like_knight_character:RUN");
}

DEFAULT_EXIT(BrawlerCharacterFSM, Running);

void BrawlerCharacterFSM::Running::Run(BrawlerCharacterFSM::StateComponent& state_)
{
	auto&& [sprite, input, player, transform, movable] = Engine::Registry().get<Sprite, InputReceiver, Player, Transform, Movable>(state_.entity);

	Float32 run = input.Get("run");

	if (!movable.isOnGround || EPSILON_NOT_ZERO(input.Get("jump")))
	{
		Logger::info(movable.isOnGround);
		Logger::info(input.Get("jump"));
		GoTo(ECharacterStates::Jumping, state_);
	}
	else if (EPSILON_ZERO(run))
	{
		GoTo(ECharacterStates::Idle, state_);
	}

	if (!EPSILON_ZERO(run))
	{
		sprite.scale.x = run;
		sprite.position.x += player.speed * sprite.scale.x * Engine::DeltaTime();
		movable.speed.x += run * player.speed;
	}
	Logger::info(run);
	Logger::info(sprite.position.x);
}

// Jumping

void BrawlerCharacterFSM::Jumping::Enter(BrawlerCharacterFSM::StateComponent& state_)
{
	auto& animator = Engine::Registry().get<Animator>(state_.entity);
	AnimatorPlay(animator, "souls_like_knight_character:JUMP");
}

DEFAULT_EXIT(BrawlerCharacterFSM, Jumping);

void BrawlerCharacterFSM::Jumping::Run(BrawlerCharacterFSM::StateComponent& state_)
{
	auto&& [sprite, input, player, transform, movable] = Engine::Registry().get<Sprite, InputReceiver, Player, Transform, Movable>(state_.entity);

	Float32 run = input.Get("run");
	
	if (movable.isOnGround)
	{
		if (EPSILON_ZERO(run))
		{
			GoTo(ECharacterStates::Idle, state_);
		}
		else
		{
			GoTo(ECharacterStates::Running, state_);
		}
	}

	//if (EPSILON_ZERO(run))
	//{
	//	GoTo(ECharacterStates::Idle, state_);
	//}
	//else
	//{
		// Logger::info(sprite.position.x);
		// Logger::info(sprite.position.y);
		//sprite.scale.x = run;
		//sprite.position.x += player.speed * sprite.scale.x * Engine::DeltaTime();
		// sprite.position.y += 50 * Engine::DeltaTime();
		if(EPSILON_NOT_ZERO(input.Get("jump")))
		{
			movable.speed.y += 50;
		}
		if (EPSILON_NOT_ZERO(run))
		{
			sprite.scale.x = run;
			movable.speed.x += run * player.speed;
		}
		// Logger::info(sprite.position.x);
		// Logger::info(sprite.position.y);
		//GoTo(ECharacterStates::Idle, state_);
	//}

}