#include "brawler_character_fsm.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "gameplay/brawler/brawler_character_controller.h"
#include "gameplay/brawler/components/movable.h"
#include "gameplay/brawler/systems/physics.h"

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

	if (!movable.isOnGround)
	{
		GoTo(ECharacterStates::Jumping, state_);
		return;
	}

	if (movable.isOnGround && EPSILON_NOT_ZERO(input.Get("jump"))) {
		movable.speed.y += PhysicsSystem::s_JumpSpeed;
		GoTo(ECharacterStates::Jumping, state_);
		return;
	}

	if (EPSILON_NOT_ZERO(input.Get("run")))
	{
		GoTo(ECharacterStates::Running, state_);
		return;
	}

	// Push the player, just for testing drag
	if (EPSILON_NOT_ZERO(input.Get("light")))
	{
		movable.speed.x += sprite.scale.x * 100.0f;
		movable.speed.y += 200.0f;
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

	if (!movable.isOnGround)
	{
		GoTo(ECharacterStates::Jumping, state_);
		return;
	}

	if (movable.isOnGround && EPSILON_NOT_ZERO(input.Get("jump")))
	{
		movable.speed.y += PhysicsSystem::s_JumpSpeed;
		GoTo(ECharacterStates::Jumping, state_);
		return;
	}

	if (EPSILON_ZERO(run))
	{
		GoTo(ECharacterStates::Idle, state_);
		return;
	} 
	else
	{
		sprite.scale.x = run;
		transform.position.x += run * PhysicsSystem::s_RunSpeed * Engine::DeltaTime();
	}

	// TODO decrease x speed when running in opposite direction
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
		return;
	}

 	auto jump = input.Get("jump");

	if(EPSILON_NOT_ZERO(jump))
	{
		// Jump longer by holding jump button
		movable.speed.y = movable.speed.y < PhysicsSystem::s_JumpSpeed/2 ? movable.speed.y : PhysicsSystem::s_JumpSpeed/2;
	}
		
	if (EPSILON_NOT_ZERO(run))
	{
		sprite.scale.x = run;
		transform.position.x += run * PhysicsSystem::s_AirMobility * PhysicsSystem::s_RunSpeed * Engine::DeltaTime();
	}
}