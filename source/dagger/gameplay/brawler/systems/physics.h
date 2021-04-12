#pragma once

#include "core/core.h"
#include "core/system.h"

using namespace dagger;

namespace brawler {


class PhysicsSystem : public System
{
public:
	String SystemName() override {
		return "Physics System";
	}

	void Run() override;

};

}