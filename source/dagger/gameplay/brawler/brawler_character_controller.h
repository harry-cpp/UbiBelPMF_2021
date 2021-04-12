#pragma once

#include "core/core.h"
#include "core/system.h"
#include "gameplay/brawler/brawler_character_fsm.h"

using namespace dagger;

namespace brawler
{
	struct Player
	{
		int speed{ 1 };
	};

	class BrawlerControllerSystem
		: public System
	{
		BrawlerCharacterFSM characterFSM;

	public:
		String SystemName() override {
			return "Character Controller System";
		}

		void Run() override;
	};
}