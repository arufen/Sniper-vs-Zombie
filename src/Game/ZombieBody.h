//---------------------------------------------------------------------------
//!	@file	ZombieBody.h
//! @brief	builds the shared zombie body: model + anims + 3 capsules(ゾンビ共通の体を作る: モデル+アニメ+カプセル3つ)
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include <memory>

namespace SniperVsZombie {

//the capsules a zombie type may need to wire up later(ゾンビの種類ごとに後で使うカプセル)
struct ZombieBodyParts
{
    std::shared_ptr<ComponentCollisionCapsule> feet;    //uses gravity, keeps it standing(重力あり。立たせる用)
    std::shared_ptr<ComponentCollisionCapsule> body;    //body hit(胴体の当たり判定)
    std::shared_ptr<ComponentCollisionCapsule> head;    //headshot check(ヘッドショット判定)
};

//add model, animations and capsules to a zombie. call it in the zombie's Init()(ゾンビにモデル・アニメ・カプセルを付ける。ゾンビのInitで呼ぶ)
//every zombie type uses this, so a fix to the hitboxes fixes all of them(全ゾンビがこれを使うので、当たり判定の修正が全種類に効く)
ZombieBodyParts AddZombieBody(Object* owner);

}    // namespace SniperVsZombie
