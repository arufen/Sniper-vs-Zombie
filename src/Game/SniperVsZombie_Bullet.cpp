//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Bullet.cpp
//! @brief	SniperVsZombie_Bullet
//---------------------------------------------------------------------------

#include "SniperVsZombie_Bullet.h"
#include "ObjectNames.h"
#include "SniperVsZombie_MainStage.h"
#include "StageConfig.h"
#include <System/Component/ComponentCollisionLine.h>

namespace SniperVsZombie {
//! @brief 初期化
//! @return 初期化終了
bool Bullet::Init()
{
    Super::Init();

    SetName(Names::BULLET);

    auto model = AddComponent<ComponentModel>("data/Game/Models/Bullet/bullet.mv1");
    model->SetScaleAxisXYZ({0.05f});

    auto col = AddComponent<ComponentCollisionLine>();

    //WORLD-SPACE line = the path flown THIS frame, from the old position to the new one (set in Update)(ワールド座標の線=このフレームで飛んだ道。古い位置から新しい位置まで。Updateで設定)
    //the old local line was scaled by SetScaleAxisXYZ(0.2) to only 50 units and started at the bullet,(以前のローカル線はスケール0.2で50しかなく、弾の位置から始まっていたので)
    //so the first 50 units in front of the gun were never checked = close stalkers were missed(銃の前の最初の50が判定されず、近くのストーカーに当たらなかった)
    col->SetCollisionStatus(ComponentCollision::CollisionBit::DisableTransform, true);
    col->ShowInGame(show_hitbox_);
    SetPathLine(GetTranslate(), GetTranslate());

    col->SetCollisionGroup(ComponentCollision::CollisionGroup::ETC);
    col->SetHitCollisionGroup((u32)ComponentCollision::CollisionGroup::ENEMY /* | 
									(u32)ComponentCollision::CollisionGroup::GROUND*/
    );

    SetScaleAxisXYZ({0.2f});
    return true;
}

void Bullet::Update()
{
    //WALL (2nd frame): last frame's path ended on the wall and nothing in front of it was hit. show the hit, remove(壁(2フレーム目): 前フレームの道が壁で終わり、手前の敵には当たらなかった。エフェクトを出して消す)
    if(reached_wall_) {
        SpawnHitEffect();
        Scene::Object::Release(SharedThis());
        return;
    }

    const float3 from = GetTranslate();
    float3       to   = from + direction_ * speed_;

    //WALL (1st frame): do not fly past the wall. the path still runs this frame, so enemies right in front of the wall get hit(壁(1フレーム目): 壁より先へ飛ばない。道の判定はこのフレームも行うので、壁のすぐ手前の敵には当たる)
    if(length(to - start_).x >= stop_distance_) {
        to            = start_ + direction_ * stop_distance_;
        reached_wall_ = true;
    }

    SetTranslate(to);
    SetPathLine(from, to);

    //Destroy this object after 5 seconds
    time_to_destroy_--;

    if(time_to_destroy_ <= 0) {
        Scene::Object::Release(SharedThis());
    }
}

void Bullet::SetDirection(float3 dir)
{
    direction_ = normalize(dir);

    SetRotationToVector(direction_);
}

void Bullet::ResetDirection()
{
    SetRotationToVector(direction_);
}

void Bullet::SetStopDistance(float distance)
{
    start_         = GetTranslate();
    stop_distance_ = distance;
    SetPathLine(
        start_,
        start_);    //Init may have run before SetTranslate, so start the line here again(SetTranslateより前にInitが走った可能性があるので、ここで線を合わせ直す)
}

void Bullet::SetPathLine(const float3& from, const float3& to)
{
    auto col = GetComponent<ComponentCollisionLine>();
    if(!col)
        return;
    //a zero-length line has no direction (SetLine builds a matrix from it), so keep it at least a tiny bit long(長さ0の線は向きがない(SetLineが向きから行列を作る)ので、最低でも少しだけ長くする)
    float3 end = to;
    if(length(to - from).x < 0.01f)
        end = from + direction_ * 0.01f;
    col->SetLine(from, end);
}

void Bullet::SetShowHitbox(bool show)
{
    show_hitbox_ = show;
}

bool Bullet::CanReach(const float3& point) const
{
    return length(point - start_).x <= stop_distance_ + Config::SHOT_WALL_TOLERANCE;
}

void Bullet::SpawnHitEffect()
{
    auto mat = GetWorldMatrix();
    auto rot = matrix::rotateY(-0.5 * DX_PI);
    mat      = mul(rot, mat);

    ComponentEffect::Object::Create("data/Sample/Effects/hit_eff.efkefc", mat);
}

// 当たり判定が行われたときに呼ばれる関数
void Bullet::OnHit(const ComponentCollision::HitInfo& hit_info)
{
    //the collision line reaches far ahead, so it can touch a zombie BEHIND a wall. ignore that one(当たり判定の線は前方に長いので、壁の向こうのゾンビに触れることがある。それは無視)
    if(const Object* other = hit_info.hit_collision_->GetOwner(); other && !CanReach(other->GetTranslate()))
        return;

    // 自分を削除する
    Scene::Object::Release(SharedThis());

    SpawnHitEffect();

    // 最後にこれを入れてください。ここでめり込みの解消を行っています。
    Super::OnHit(hit_info);
}

}    // namespace SniperVsZombie
