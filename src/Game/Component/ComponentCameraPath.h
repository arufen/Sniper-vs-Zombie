//---------------------------------------------------------------------------
//!	@file	ComponentCameraPath.h
//! @brief	cutscene camera: plays camera shots one after another, with fade cuts between them(カットシーンカメラ: ショットを順番に再生し、間をフェードでつなぐ)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <System/Component/ComponentCamera.h>
#include <Game/StageConfig.h>
#include <functional>
#include <vector>

USING_PTR(ComponentCameraPath);

//how to use(使い方):
//  auto obj  = Scene::Object::Create<Object>();
//  auto path = obj->AddComponent<ComponentCameraPath>();   //adds a ComponentCamera too(ComponentCameraも追加する)
//  path->SetShots(Config::OPENING_SHOTS)->SetOnFinished([this]{ ... })->Play();
//  in PostDraw: path->DrawFade();(PostDrawでDrawFadeを呼ぶ)
//inside one shot the camera blends A -> B with ease in/out. between shots there is a fade cut(1ショット内はA→Bをイーズで補間。ショット間はフェードカット)
class ComponentCameraPath : public Component
{
public:
    BP_COMPONENT_DECL(ComponentCameraPath, u8"カットシーンカメラパス");

    using CameraShot = SniperVsZombie::Config::CameraShot;

    void Init() override;

    void Update() override;

    ComponentCameraPathPtr SetShots(const std::vector<CameraShot>& shots);

    //called once when the last shot ends (or after Skip)(最後のショット終了時(またはSkip後)に1回呼ばれる)
    ComponentCameraPathPtr SetOnFinished(std::function<void()> on_finished);

    //look at this object instead of the shot targets, e.g. the helicopter(ショットの注視点の代わりにこのObjectを見る。例: ヘリ)
    ComponentCameraPathPtr SetFollowTarget(std::weak_ptr<Object> target, float3 offset = {0.0f, 0.0f, 0.0f});

    //true = start from black and fade in (use it when the scene does not fade in by itself)(true=黒から始めてフェードイン(シーン自体がフェードインしない時に使う))
    ComponentCameraPathPtr SetFadeInAtStart(bool fade_in);

    //make this the current camera and start shot 0(カレントカメラにしてショット0から開始)
    void Play();

    //jump to the end. fades from black into whatever comes next(最後へ飛ぶ。黒から次の画面へフェードする)
    void Skip();

    bool IsPlaying() const { return playing_; }

    //total time of all shots(全ショットの合計時間)
    float GetTotalTime() const;

    //time since Play()(Play()からの経過時間)
    float GetTime() const { return total_time_; }

    //black overlay for the fade cuts. call by hand in the scene's PostDraw, after the 3D world(フェードカット用の黒。シーンのPostDrawで3Dの後に手動で呼ぶ)
    void DrawFade();

    //give the view back to another camera (e.g. the FPS one) by removing our ComponentCamera(自分のComponentCameraを外して、別のカメラ(FPSなど)に視点を返す)
    void ReleaseCamera();

private:
    void EnsureCamera();
    void ApplyCamera();
    void Finish();

    std::vector<CameraShot> shots_;
    std::function<void()>   on_finished_;
    std::weak_ptr<Object>   follow_target_;
    float3                  follow_offset_ = {0.0f, 0.0f, 0.0f};
    bool                    has_follow_    = false;
    bool                    fade_in_start_ = false;

    std::weak_ptr<ComponentCamera> camera_;    //weak so we do not keep it alive(寿命を握らないようweak)

    int   shot_index_ = 0;
    float shot_time_  = 0.0f;    //time inside the current shot(今のショット内の経過時間)
    float total_time_ = 0.0f;
    bool  playing_    = false;
    float fade_alpha_ = 0.0f;    //0 = clear, 1 = black(0=透明、1=黒)
    float skip_fade_  = 0.0f;    //after Skip: fade from black, counts down(Skip後: 黒から戻すフェード。カウントダウン)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    CEREAL_SAVELOAD(arc, ver) { arc(cereal::make_nvp("Component", cereal::base_class<Component>(this))); }
};

CEREAL_CLASS_VERSION(ComponentCameraPath, 1);
