#pragma once

#include "core/system.h"
#include "core/core.h"

using namespace dagger;

namespace pandemic_shop
{
    struct ControllerMapping
    {
        EDaggerKeyboard up_key;
        EDaggerKeyboard down_key;

        Vector2 input{ 0, 0 };
    };

    class PandemicShopPlayerInputSystem
        : public System
    {
        static Float32 s_BoarderUp;
        static Float32 s_BoarderDown;

    public:

        static Float32 s_PlayerSpeed;

    public:
        inline String SystemName() { return "PandemicShop Player Input System"; }

        void SpinUp() override;
        void WindDown() override;
        void Run() override;

        static void SetupPlayerInput(ControllerMapping& controllerMapping_)
        {
            controllerMapping_.up_key = EDaggerKeyboard::KeyW;
            controllerMapping_.down_key = EDaggerKeyboard::KeyS;
        }

        static void SetupPlayerBoarders(Float32 boarderUp_, Float32 boarderDown_)
        {
            s_BoarderUp = boarderUp_;
            s_BoarderDown = boarderDown_;
        }

    private:

        void OnKeyboardEvent(KeyboardEvent kEvent_);
    };
}