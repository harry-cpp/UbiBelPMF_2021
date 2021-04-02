#include "editor_main.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/game/transforms.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/textures.h"
#include "core/graphics/animations.h"
#include "core/graphics/gui.h"
#include "tools/diagnostics.h"

#include "core/savegame.h"
#include "gameplay/editor/savegame_system.h"
#include <iostream>

using namespace dagger;
using namespace editor;

ECommonSaveArchetype EditorTestGame::Save(Entity entity_, JSON::json& saveTo_)
{
    auto& registry = Engine::Registry();

    ECommonSaveArchetype archetype = ECommonSaveArchetype::None;

    if (registry.has<Sprite>(entity_))
    {
        saveTo_["sprite"] = SerializeComponent<Sprite>(registry.get<Sprite>(entity_));
        archetype = archetype | ECommonSaveArchetype::Sprite;
    }
    
    if (registry.has<Transform>(entity_))
    {
        saveTo_["transform"] = SerializeComponent<Transform>(registry.get<Transform>(entity_));
        archetype = archetype | ECommonSaveArchetype::Transform;
    }
        
    if (registry.has<Animator>(entity_))
    {
        saveTo_["animator"] = SerializeComponent<Animator>(registry.get<Animator>(entity_));
        archetype = archetype | ECommonSaveArchetype::Animator;
    }

    return archetype;
}

void EditorTestGame::Load(ECommonSaveArchetype archetype_, Entity entity_, JSON::json& loadFrom_)
{
    auto& registry = Engine::Registry();

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Sprite))
        DeserializeComponent<Sprite>(loadFrom_["sprite"], registry.emplace<Sprite>(entity_));

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Transform))
        DeserializeComponent<Transform>(loadFrom_["transform"], registry.emplace<Transform>(entity_));

    if (IS_ARCHETYPE_SET(archetype_, ECommonSaveArchetype::Animator))
        DeserializeComponent<Animator>(loadFrom_["animator"], registry.emplace<Animator>(entity_));
}

void EditorTestGame::GameplaySystemsSetup()
{
    auto& engine = Engine::Instance();
    engine.AddSystem<EditorToolSystem>();
    engine.AddSystem<SaveGameSystem<ECommonSaveArchetype>>(this);
}

void EditorTestGame::WorldSetup()
{
    ShaderSystem::Use("standard");

    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::ShowAsMuchAsPossible;
    camera->position = { 0, 0, 0 };
    camera->Update();
}

void EditorToolSystem::OnToolMenuRender()
{
    if (ImGui::BeginMenu("Scene"))
    {
        if (ImGui::MenuItem("Clear"))
        {
            Engine::Registry().clear();
        }

        if (ImGui::MenuItem("Save"))
        {
            Engine::Dispatcher().trigger<SaveGameSystem<ECommonSaveArchetype>::SaveRequest>(
                SaveGameSystem<ECommonSaveArchetype>::SaveRequest{ "editor_test_save.json" });
        }

        if (ImGui::MenuItem("Load"))
        {
            Engine::Dispatcher().trigger<SaveGameSystem<ECommonSaveArchetype>::LoadRequest>(
                SaveGameSystem<ECommonSaveArchetype>::LoadRequest{ "editor_test_save.json" });
        }

        ImGui::EndMenu();
    }
}

void EditorToolSystem::OnKeyboardEvent(KeyboardEvent event_)
{
    if (event_.key == EDaggerKeyboard::KeyTab && event_.action == EDaggerInputState::Released)
        if (Input::IsInputDown(EDaggerKeyboard::KeyLeftControl))
            m_IsInEditor = !m_IsInEditor;
}

void EditorToolSystem::Run()
{
    if (m_IsInEditor)
    {
        auto& knob = m_Registry.get<Sprite>(m_Focus);
        auto& focus = m_Registry.get<EditorFocus>(m_Focus);

        if (Input::IsInputDown(EDaggerMouse::MouseButton3))
        {
            knob.position = Vector3{ Input::CursorPositionInWorld(), 0 };
            focus.dirty = true;
        }

        if (Input::IsInputDown(EDaggerKeyboard::KeyLeftShift))
        {
            auto& reg = Engine::Registry();
            if (reg.valid(m_Selected.entity))
            {
                auto& sprite = reg.get<Sprite>(m_Selected.entity);
                knob.position = Vector3{ Input::CursorPositionInWorld(), 0 };
                sprite.position = knob.position;
            }
        }

        if (!Input::IsInputDown(EDaggerMouse::MouseButton1) && focus.dirty)
        {
            m_Targets.clear();
            focus.dirty = false;
            Engine::Registry().view<Sprite, SaveGame<ECommonSaveArchetype>>()
                .each([&](Entity entity_, const Sprite& sprite_, const SaveGame<ECommonSaveArchetype>& save_)
                    {
                        const auto left = sprite_.position.x - (sprite_.size.x / 2) * sprite_.scale.x;
                        const auto top = sprite_.position.y - (sprite_.size.y / 2) * sprite_.scale.y;

                        const auto right = sprite_.position.x + (sprite_.size.x / 2) * sprite_.scale.x;
                        const auto bottom = sprite_.position.y + (sprite_.size.y / 2) * sprite_.scale.y;

                        if (knob.position.x >= left && knob.position.y >= top &&
                            knob.position.x <= right && knob.position.y <= bottom)
                        {
                            m_Targets.push_back(EditorFocusTarget{ entity_, sprite_.image->Name() });
                        }
                    });
        }
    }
}

void EditorToolSystem::OnRenderGUI()
{
    if (m_IsInEditor)
    {
        auto& reg = Engine::Registry();

        static int selectedItem = 0;
        Sequence<const char*> items;

        items.push_back("<none>");
        for (auto& item : m_Targets)
        {
            items.push_back(item.name.c_str());
        }

        ImGui::Begin("Entities");

        if (ImGui::Button("Create entity"))
        {
            auto newEntity = reg.create();
            auto& newSprite = reg.emplace<Sprite>(newEntity);
            AssignSprite(newSprite, "tools:knob2");
            auto& newSavegame = reg.emplace<SaveGame<ECommonSaveArchetype>>(newEntity);
            auto& focus = m_Registry.get<EditorFocus>(m_Focus);
            focus.dirty = true;
        }

        ImGui::ListBox("In Focus", &selectedItem, items.data(), items.size(), 10);

        if (selectedItem - 1 >= m_Targets.size())
        {
            selectedItem = 0;
            m_Selected.entity = Entity{ -1 };
        }

        if (selectedItem > 0)
        {
            const int index = selectedItem - 1;
            m_Selected = m_Targets[index];
            
            if (reg.has<Sprite>(m_Selected.entity) && ImGui::CollapsingHeader("Sprite"))
            {
                Sprite& compSprite = reg.get<Sprite>(m_Selected.entity);

                /* Sprite texture */ {
                    static int selectedTexture = 0;
                    Sequence<const char*> textures;
                    int i = 0;
                    int currentSelected = 0;
                    for (auto& [k, n] : Engine::Res<Texture>())
                    {
                        textures.push_back(k.c_str());
                        if (k == compSprite.image->Name())
                        {
                            selectedTexture = i;
                        }
                        ++i;
                    }

                    currentSelected = selectedTexture;
                    if (ImGui::Combo("Image", &selectedTexture, textures.data(), textures.size()))
                    {
                        if (currentSelected != selectedTexture)
                        {
                            AssignSprite(compSprite, textures[selectedTexture]);
                        }
                    }
                }

                /* Size values */ {
                    float size[]{ compSprite.size.x, compSprite.size.y };
                    ImGui::SliderFloat2("Pixel Size", size, -10000.0f, 10000.0f, "%f", 1);
                    compSprite.size.x = size[0];
                    compSprite.size.y = size[1];
                }

                /* Position values */ {
                    float pos[]{ compSprite.position.x, compSprite.position.y };
                    ImGui::InputFloat2("Position", pos, "%f", 1);
                    compSprite.position.x = pos[0];
                    compSprite.position.y = pos[1];
                }

                /* Rotation value */ {
                    ImGui::SliderFloat("Rotation", &compSprite.rotation, 0, 360, "%f", 1);
                }

                /* Scale values */ {
                    float size[]{ compSprite.scale.x, compSprite.scale.y };
                    ImGui::DragFloat2("Scale", size, 1, -10, 10, "%f", 1);
                    compSprite.scale.x = size[0];
                    compSprite.scale.y = size[1];
                }

                /* Pivot values */ {
                    float pivot[]{ compSprite.pivot.x, compSprite.pivot.y };
                    ImGui::DragFloat2("Pivot", pivot, 1, -0.5f, 0.5f, "%f", 1);
                    compSprite.pivot.x = pivot[0];
                    compSprite.pivot.y = pivot[1];
                }
            }

            if (reg.has<Animator>(m_Selected.entity) && ImGui::CollapsingHeader("Animator"))
            {
                ImGui::Text("Animator");
            }
        }

        ImGui::End();
    }
}