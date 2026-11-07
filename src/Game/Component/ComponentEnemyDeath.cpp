#include <Game/Component/ComponentEnemyDeath.h>
#include <Game/SniperVsZombie_MainStage.h>

void ComponentEnemyDeath::Init()
{
    __super::Init();

    SetName<Component>("EnemyDeath");
}

void ComponentEnemyDeath::Update()
{
    __super::Update();

    //count down the dead body time(死体の残り時間を減らす)
    if(is_dead_)
        corpse_time_ -= static_cast<float>(GetDeltaTime());
}

ComponentEnemyDeathPtr ComponentEnemyDeath::SetHeadCollision(const std::shared_ptr<ComponentCollision>& head)
{
    head_collision_ = head;
    return std::dynamic_pointer_cast<ComponentEnemyDeath>(shared_from_this());
}

ComponentEnemyDeathPtr ComponentEnemyDeath::SetScore(int body_score, int head_score)
{
    body_score_ = body_score;
    head_score_ = head_score;
    return std::dynamic_pointer_cast<ComponentEnemyDeath>(shared_from_this());
}

ComponentEnemyDeathPtr ComponentEnemyDeath::SetCorpseTime(float seconds)
{
    corpse_time_ = seconds;
    return std::dynamic_pointer_cast<ComponentEnemyDeath>(shared_from_this());
}

ComponentEnemyDeathPtr ComponentEnemyDeath::SetOnDie(std::function<void()> on_die)
{
    on_die_ = on_die;
    return std::dynamic_pointer_cast<ComponentEnemyDeath>(shared_from_this());
}

void ComponentEnemyDeath::OnBulletHit(const ComponentCollision::HitInfo& hit_info)
{
    //collision_ is OUR capsule that got hit. if it is the head capsule, it is a headshot(collision_は被弾した自分のカプセル。頭のカプセルならヘッドショット)
    was_headshot_ = (hit_info.collision_ == head_collision_.lock());

    if(!is_dead_)
        Die();

    //hit marker on the screen(画面のヒットマーカー)
    if(auto scene = dynamic_cast<SniperVsZombie::SniperVsZombie_MainStage*>(Scene::GetCurrentScene())) {
        scene->TriggerHitmark(was_headshot_);
    }
}

void ComponentEnemyDeath::Die()
{
    is_dead_ = true;

    auto owner = GetOwner();

    //play the hit effect at this enemy's position, and add the score(敵の位置でヒットエフェクトを出して、スコアを加算)
    if(auto scene = dynamic_cast<SniperVsZombie::SniperVsZombie_MainStage*>(Scene::GetCurrentScene())) {
        int    play_handle = PlayEffekseer3DEffect(scene->GetHitEffectHandle());
        float3 pos         = owner->GetTranslate();
        SetPosPlayingEffekseer3DEffect(play_handle, pos.x, pos.y + 10.0f, pos.z);
        SetScalePlayingEffekseer3DEffect(play_handle, 5.0f, 5.0f, 5.0f);

        scene->AddScore(was_headshot_ ? head_score_ : body_score_);
    }

    if(auto model = owner->GetComponent<ComponentModel>())
        model->PlayAnimationNoSame("dead", false);

    //let the Object stop its walking / attacking(Objectに歩きや攻撃を止めてもらう)
    if(on_die_)
        on_die_();

    //there are 3 capsules (feet, body, head), RemoveComponent only removes one at a time, so loop until all are gone(カプセルは3つあり、RemoveComponentは1つずつなので、全部消えるまでループ)
    while(auto col = owner->GetComponent<ComponentCollisionCapsule>()) owner->RemoveComponent(col);
}

void ComponentEnemyDeath::GUI()
{
    __super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Enemy Death")) {
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            ImGui::Text(u8"死亡: %s", is_dead_ ? "true" : "false");
            ImGui::Text(u8"ヘッドショット: %s", was_headshot_ ? "true" : "false");

            ImGui::DragInt(u8"胴体スコア", &body_score_, 1, 0, 100);
            ImGui::DragInt(u8"ヘッドショットスコア", &head_score_, 1, 0, 100);
            ImGui::DragFloat(u8"死体が残る時間(秒)", &corpse_time_, 0.1f);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentEnemyDeath)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentEnemyDeath)
