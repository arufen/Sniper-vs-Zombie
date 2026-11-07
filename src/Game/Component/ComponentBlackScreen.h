#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <functional>

USING_PTR(ComponentBlackScreen);

class ComponentBlackScreen : public Component
{
public:
    BP_COMPONENT_DECL(ComponentBlackScreen, u8"ブラックスクリーン遷移コンポーネント");

    void IncreaseAlpha();
    void DecreaseAlpha();
    bool isFullyOpaque() const { return alpha_ >= 255; }
    bool isFullyClear() const { return alpha_ <= 0; }

    //! @brief starts fading TO black. once fully black, onComplete fires ONCE - use it to Scene::Change.
    void StartFadeOut(std::function<void()> onComplete);

    //! @brief starts fading FROM black back to clear. call this right after a new scene starts (in Init()).
    void StartFadeIn();

    void Init() override;

    void Update() override;

    //! @brief actually draws the black overlay box. call this manually at the very END
    //! of your scene's PostDraw(), after everything else, so it covers the whole screen.
    void DrawBlackScreen();

    void GUI() override;

private:
    enum class State
    {
        Idle,
        FadingOut,    //!< alpha rising to 255, then fires the stored callback once
        FadingIn,     //!< alpha falling back down to 0
    };

    State                 fade_state_ = State::Idle;
    std::function<void()> on_fade_out_complete_;

    int       alpha_          = 0;        //!< 透明度
    const int alpha_speed_    = 2;        //!< 透明度の変化速度
    bool      isInTransition_ = false;    //!< 透明度変化中かどうか

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //! @brief セーブ
    // @param arc アーカイバ
    // @param ver バージョン
    CEREAL_SAVELOAD(arc, ver) { arc(cereal::make_nvp("Component", cereal::base_class<Component>(this))); }
};

CEREAL_CLASS_VERSION(ComponentBlackScreen, 4);
