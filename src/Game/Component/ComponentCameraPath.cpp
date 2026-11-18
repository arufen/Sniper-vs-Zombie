//---------------------------------------------------------------------------
//!	@file	ComponentCameraPath.cpp
//! @brief	cutscene camera path(カットシーンカメラパス)
//---------------------------------------------------------------------------
#include <Game/Component/ComponentCameraPath.h>
#include <algorithm>

namespace Config = SniperVsZombie::Config;    //components live outside the game namespace(コンポーネントはゲームの名前空間の外にある)

namespace {
//ease in/out: slow start, slow end(イーズイン/アウト: ゆっくり始まり、ゆっくり止まる)
float SmoothStep(float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

float3 Lerp(const float3& a, const float3& b, float t)
{
    return a + (b - a) * t;
}
}    // namespace

void ComponentCameraPath::Init()
{
    __super::Init();

    EnsureCamera();
}

void ComponentCameraPath::EnsureCamera()
{
    //made here, not only in Init: the engine runs Init one frame LATER, but scenes call Play() right after AddComponent(Initだけでなくここで作る: エンジンはInitを1フレーム後に呼ぶが、シーンはAddComponent直後にPlayを呼ぶ)
    if(!camera_.expired())
        return;

    //the owner stays at the origin with no rotation, because ComponentCamera positions are RELATIVE to the owner(所有Objectは原点・無回転のまま。ComponentCameraの位置は所有Objectからの相対だから)
    //so eye/target here = world positions(なのでここのeye/target=ワールド座標)
    auto owner = GetOwner();
    owner->SetTranslate({0.0f, 0.0f, 0.0f});
    owner->SetRotationAxisXYZ({0.0f, 0.0f, 0.0f});

    auto camera = owner->GetComponent<ComponentCamera>();
    if(!camera)
        camera = owner->AddComponent<ComponentCamera>();
    camera->SetPerspective(60.0f);    //same FOV as the FPS camera, so O3 -> player view does not jump(FPSカメラと同じ画角。O3→プレイヤー視点で画角が飛ばない)
    camera->SetNearFar(0.1f, 3000.0f);
    camera_ = camera;
}

void ComponentCameraPath::ReleaseCamera()
{
    //remove only the camera: the FPS camera takes over, but DrawFade still works for the skip fade(カメラだけ外す: FPSカメラが引き継ぐが、スキップのフェードはDrawFadeで続く)
    if(auto camera = camera_.lock())
        GetOwner()->RemoveComponent(camera);
    camera_.reset();
}

ComponentCameraPathPtr ComponentCameraPath::SetShots(const std::vector<CameraShot>& shots)
{
    shots_ = shots;
    return std::dynamic_pointer_cast<ComponentCameraPath>(shared_from_this());
}

ComponentCameraPathPtr ComponentCameraPath::SetOnFinished(std::function<void()> on_finished)
{
    on_finished_ = on_finished;
    return std::dynamic_pointer_cast<ComponentCameraPath>(shared_from_this());
}

ComponentCameraPathPtr ComponentCameraPath::SetFollowTarget(std::weak_ptr<Object> target, float3 offset)
{
    follow_target_ = target;
    follow_offset_ = offset;
    has_follow_    = true;
    return std::dynamic_pointer_cast<ComponentCameraPath>(shared_from_this());
}

ComponentCameraPathPtr ComponentCameraPath::SetFadeInAtStart(bool fade_in)
{
    fade_in_start_ = fade_in;
    return std::dynamic_pointer_cast<ComponentCameraPath>(shared_from_this());
}

float ComponentCameraPath::GetTotalTime() const
{
    float total = 0.0f;
    for(const auto& shot : shots_) total += shot.duration;
    return total;
}

void ComponentCameraPath::Play()
{
    shot_index_ = 0;
    shot_time_  = 0.0f;
    total_time_ = 0.0f;
    skip_fade_  = 0.0f;
    playing_    = !shots_.empty();

    EnsureCamera();
    if(auto camera = camera_.lock())
        camera->SetCurrentCamera();

    if(playing_) {
        ApplyCamera();
    }
    else if(on_finished_) {
        on_finished_();    //no shots = nothing to play(ショットが無い=再生する物が無い)
    }
}

void ComponentCameraPath::Skip()
{
    if(!playing_)
        return;

    //show the last frame of the last shot, then finish(最後のショットの最終フレームを見せて終了)
    shot_index_ = static_cast<int>(shots_.size()) - 1;
    shot_time_  = shots_.back().duration;
    ApplyCamera();

    skip_fade_ = Config::CUT_FADE_TIME * 0.5f;    //come back from black, so the skip is not a hard pop(黒から戻す。スキップが急に切り替わらないように)
    Finish();
}

void ComponentCameraPath::Update()
{
    __super::Update();

    const float dt = GetDeltaTime();

    if(skip_fade_ > 0.0f)
        skip_fade_ = std::max(0.0f, skip_fade_ - dt);

    if(!playing_)
        return;

    shot_time_  += dt;
    total_time_ += dt;

    //next shot(次のショット)
    while(playing_ && shot_time_ >= shots_[shot_index_].duration) {
        shot_time_ -= shots_[shot_index_].duration;
        shot_index_++;
        if(shot_index_ >= static_cast<int>(shots_.size())) {
            shot_index_ = static_cast<int>(shots_.size()) - 1;
            shot_time_  = shots_[shot_index_].duration;
            ApplyCamera();
            Finish();
            return;
        }
    }

    ApplyCamera();
}

void ComponentCameraPath::ApplyCamera()
{
    const CameraShot& shot = shots_[shot_index_];
    const float       t    = SmoothStep(shot.duration > 0.0f ? shot_time_ / shot.duration : 1.0f);

    const float3 eye    = Lerp(shot.eye_a, shot.eye_b, t);
    float3       target = Lerp(shot.target_a, shot.target_b, t);

    if(has_follow_) {
        if(auto obj = follow_target_.lock())
            target = obj->GetTranslate() + follow_offset_;
    }

    if(auto camera = camera_.lock())
        camera->SetPositionAndTarget(eye, target);

    //FADE CUT: half fade at the end of a shot (if another comes), half at the start (if one came before)(フェードカット: 次があれば最後に半分、前があれば最初に半分)
    const float half       = Config::CUT_FADE_TIME * 0.5f;
    const bool  has_next   = shot_index_ + 1 < static_cast<int>(shots_.size());
    const bool  has_before = shot_index_ > 0 || fade_in_start_;
    float       alpha      = 0.0f;
    if(half > 0.0f) {
        if(has_before)
            alpha = std::max(alpha, 1.0f - shot_time_ / half);
        if(has_next)
            alpha = std::max(alpha, 1.0f - (shot.duration - shot_time_) / half);
    }
    fade_alpha_ = std::clamp(alpha, 0.0f, 1.0f);
}

void ComponentCameraPath::Finish()
{
    playing_    = false;
    fade_alpha_ = 0.0f;

    //copy first: the callback may remove this component's owner(先にコピー: コールバックがこのObjectを消すかもしれない)
    auto callback = on_finished_;
    if(callback)
        callback();
}

void ComponentCameraPath::DrawFade()
{
    float alpha = fade_alpha_;
    if(skip_fade_ > 0.0f && Config::CUT_FADE_TIME > 0.0f)
        alpha = std::max(alpha, skip_fade_ / (Config::CUT_FADE_TIME * 0.5f));

    if(alpha <= 0.0f)
        return;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.0f * alpha));
    DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

CEREAL_REGISTER_TYPE(ComponentCameraPath)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentCameraPath)
