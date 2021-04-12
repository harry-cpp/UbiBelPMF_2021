#include "brawler_character_controller.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"

using namespace brawler;

void BrawlerControllerSystem::Run()
{
    Engine::Registry().view<BrawlerCharacterFSM::StateComponent>()
        .each([&](BrawlerCharacterFSM::StateComponent& state_)
            {
                characterFSM.Run(state_);
            });
}
