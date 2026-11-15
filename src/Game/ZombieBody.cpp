//---------------------------------------------------------------------------
//!	@file	ZombieBody.cpp
//! @brief	ZombieBody
//---------------------------------------------------------------------------
#include "ZombieBody.h"

namespace SniperVsZombie {

static const char* ZOMBIE_MODEL_PATH = "data/Game/Models/Enemy/Enemy.mv1";

ZombieBodyParts AddZombieBody(Object* owner)
{
    ZombieBodyParts parts;

    //MODEL + ANIMS. add a new Mixamo anim here ONLY after the .mv1 file really exists(モデルとアニメ。Mixamoの新アニメは.mv1ができてから追加)
    auto model = owner->AddComponent<ComponentModel>(ZOMBIE_MODEL_PATH);
    model->SetAnimation({
        {"attack",         "data/Game/Models/Enemy/Anims/Attack.mv1", 0, 1.0f}, // Attack
        {  "walk",           "data/Game/Models/Enemy/Anims/Walk.mv1", 0, 3.0f}, // Walk
        {  "dead",           "data/Game/Models/Enemy/Anims/Dead.mv1", 0, 1.0f}, // Dead
        {  "jump", "data/Game/Models/Enemy/Anims/Jump_NoInPlace.mv1", 0, 1.5f}, // Jump
    });

    //FEET - to stand(足。立つ用)
    parts.feet = owner->AddComponent<ComponentCollisionCapsule>();
    parts.feet->SetRadius(3.0f)->SetHeight(10.0f)->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY)->UseGravity();

    //BODY - follows the hips bone(胴体。腰のボーンに追従)
    parts.body = owner->AddComponent<ComponentCollisionCapsule>();
    parts.body->SetRadius(2.0f)->SetHeight(9.0f)->SetTranslate({5.0f, -35.0f, 0.0f})->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
    parts.body->AttachToModel("mixamorig:Hips");

    //HEAD - follows the head bone(頭。頭のボーンに追従)
    parts.head = owner->AddComponent<ComponentCollisionCapsule>();
    parts.head->SetRadius(3.0f)->SetHeight(5.0f)->SetTranslate({0.0f, -11.0f, 0.0f})->SetCollisionGroup(ComponentCollision::CollisionGroup::ENEMY);
    parts.head->AttachToModel("mixamorig:Head");

    return parts;
}

}    // namespace SniperVsZombie
