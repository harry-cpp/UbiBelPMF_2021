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
        EDaggerKeyboard right_key;
        EDaggerKeyboard left_key;
        
        Vector2 input{ 0, 0};
    };

    class PandemicShopPlayerInputSystem
        : public System
    {
        static Float32 s_BoarderUp;
        static Float32 s_BoarderDown;
        static Float32 s_BoarderRight;
        static Float32 s_BoarderLeft;

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
            controllerMapping_.right_key = EDaggerKeyboard::KeyD;
            controllerMapping_.left_key = EDaggerKeyboard::KeyA;
        }

        static void SetupPlayerBoarders(Float32 boarderUp_, Float32 boarderDown_,
                                        Float32 boarderRight_, Float32 boarderLeft_)
        {
            s_BoarderUp = boarderUp_;
            s_BoarderDown = boarderDown_;
            s_BoarderRight = boarderRight_;
            s_BoarderLeft = boarderLeft_;
        }

    private:

        void OnKeyboardEvent(KeyboardEvent kEvent_);
    };
}