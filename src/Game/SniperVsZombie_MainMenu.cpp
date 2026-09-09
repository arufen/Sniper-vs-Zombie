//---------------------------------------------------------------------------
//!	@file	Tutorial_X.cpp
//! @brief	Tutorial_X
//---------------------------------------------------------------------------
#include "SniperVsZombie_MainMenu.h"
#include "SniperVsZombie_City.h"
#include "SniperVsZombie_WoodTower.h"
#include "SniperVsZombie_MainStage.h"
#include "Hit.h"
#include <System/Component/ComponentCamera.h>

namespace SniperVsZombie {
const float3 CAMERA_SPAWN_POSITION = float3{359.585f, 146.337f, 88.709f};
//! @brief 初期化
//! @return 初期化済み
bool SniperVsZombie_MainMenu::Init()
{
    //CAMERA
    //Scene::Object::Create<Camera>()->SetTranslate(CAMERA_SPAWN_POSITION);
    auto camera     = Scene::Object::Create<Object>();
    auto cameraComp = camera->AddComponent<ComponentCamera>();
    cameraComp->SetPositionAndTarget({66.70f, -11.70f, -57.40f}, {0.0f, 0.0f, 1.0f});
    cameraComp->SetPerspective(60.0f);
    camera->SetTranslate(CAMERA_SPAWN_POSITION);
    camera->SetName("Camera");

    //MAP
    Scene::Object::Create<City>();
    auto   woodTower     = Scene::Object::Create<WoodTower>();
    float3 watchTowerPos = {437.0f, 100.0f, 550.0f};
    woodTower->SetTranslate({watchTowerPos});
    woodTower->SetRotationAxisXYZ({0.0f, -90.0f, 0.0f});

    //Image
    titleHandle = LoadGraph("data/Game/UI/title.png");
    return true;
}

void SniperVsZombie_MainMenu::UpdateButton(Float2& pos, Float2& size, float& scale, bool& outHovered)
{
    Float2 mousePos = GetMouseFloat2();
    outHovered      = CheckPointBoxHit(mousePos, pos, size);

    // grow toward BUTTON_HOVER_SCALE while hovered, shrink back to 1.0 otherwise
    float targetScale  = outHovered ? BUTTON_HOVER_SCALE : 1.0f;
    scale             += (targetScale - scale) * BUTTON_SCALE_SPEED * GetDeltaTime();
}

void SniperVsZombie_MainMenu::DrawButton(Float2& pos, Float2& size, float scale, const char* label, int color)
{
    // scale outward from the button's center so it grows evenly, not just down-right
    Float2 center     = {pos.x + size.x * 0.5f, pos.y + size.y * 0.5f};
    Float2 scaledSize = {size.x * scale, size.y * scale};
    Float2 scaledPos  = {center.x - scaledSize.x * 0.5f, center.y - scaledSize.y * 0.5f};

    DrawBoxAA(scaledPos.x, scaledPos.y, scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y, color, 1);

    DrawFormatString(static_cast<int>(center.x - 20.0f), static_cast<int>(center.y - 8.0f), GetColor(255, 255, 255), "%s", label);
}

void SniperVsZombie_MainMenu::Update()
{
    __super::Update();

    bool playHovered = false;
    bool exitHovered = false;
    UpdateButton(playButtonPos_, playButtonSize_, playButtonScale_, playHovered);
    UpdateButton(exitButtonPos_, exitButtonSize_, exitButtonScale_, exitHovered);

    int  mouseInput   = GetMouseInput();
    bool leftDown     = (mouseInput & MOUSE_INPUT_LEFT) != 0;
    bool leftClicked  = leftDown && !wasLeftMouseDown_;    // only fire once, on the frame the click starts
    wasLeftMouseDown_ = leftDown;

    if(leftClicked) {
        if(playHovered) {
            Scene::Change(Scene::GetScene<SniperVsZombie_MainStage>());
        }
        else if(exitHovered) {
            ::PostMessage(DxLib::GetMainWindowHandle(), WM_CLOSE, 0, 0);
        }
    }
}

void SniperVsZombie_MainMenu::PostDraw()
{
    __super::PostDraw();
    DrawGraph(0, 0, titleHandle, TRUE);

    DrawButton(playButtonPos_, playButtonSize_, playButtonScale_, "PLAY", GetColor(0, 200, 0));
    DrawButton(exitButtonPos_, exitButtonSize_, exitButtonScale_, "EXIT", GetColor(200, 0, 0));
}

}    // namespace SniperVsZombie
