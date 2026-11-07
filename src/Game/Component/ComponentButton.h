#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <Game/Float2.h>
#include <Game/Hit.h>
#include <functional>
#include <string>

USING_PTR(ComponentButton);

class ComponentButton : public Component
{
public:
    BP_COMPONENT_DECL(ComponentButton, u8"ボタンコンポーネント");

    //! @brief sets up the button. call this once, right after AddComponent<ComponentButton>().
    void Setup(const Float2& pos, const Float2& size, const std::string& label, int color, std::function<void()> onClick);

    void Init() override;

    void Update() override;

    //! @brief actually draws the button + label. call this manually from your scene's PostDraw(),
    //! same way ComponentBlackScreen::DrawBlackScreen() is called manually.
    void DrawButton();

    void GUI() override;

private:
    Float2 pos_   = {0.0f, 0.0f};
    Float2 size_  = {100.0f, 40.0f};
    float  scale_ = 1.0f;

    std::string label_;
    int         color_ = 0;

    std::function<void()> on_click_;

    bool wasLeftMouseDown_ = false;    //!< used to detect the frame a click starts, not held-down spam

    static constexpr float HOVER_SCALE = 1.15f;    //!< how big it grows on hover
    static constexpr float SCALE_SPEED = 8.0f;     //!< how fast it grows/shrinks (higher = snappier)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //! @brief セーブ
    // @param arc アーカイバ
    // @param ver バージョン
    CEREAL_SAVELOAD(arc, ver) { arc(cereal::make_nvp("Component", cereal::base_class<Component>(this))); }
};

CEREAL_CLASS_VERSION(ComponentButton, 1);
