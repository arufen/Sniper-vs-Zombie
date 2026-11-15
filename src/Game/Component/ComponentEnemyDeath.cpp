#include <Game/Component/ComponentEnemyDeath.h>

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

ComponentEnemyDeathPtr ComponentEnemyDeath::SetOnKilled(std::function<void(const KillInfo&)> on_killed)
{
    on_killed_ = on_killed;
    return std::dynamic_pointer_cast<ComponentEnemyDeath>(shared_from_this());
}

void ComponentEnemyDeath::OnBulletHit(const ComponentCollision::HitInfo& hit_info)
{
    //already dead, the corpse does not count again(もう死んでいる。死体は二重にカウントしない)
    if(is_dead_)
        return;

    //collision_ is OUR capsule that got hit. if it is the head capsule, it is a headshot(collision_は被弾した自分のカプセル。頭のカプセルならヘッドショット)
    was_headshot_ = (hit_info.collision_ == head_collision_.lock());

    Die();
}

void ComponentEnemyDeath::Die()
{
    is_dead_ = true;

    auto owner = GetOwner();

    //tell the game. whoever spawned us decides what a kill means (score, hit marker, effect)(ゲームに伝える。キルの意味は生成した側が決める)
    if(on_killed_) {
        KillInfo info;
        info.headshot = was_headshot_;
        info.score    = was_headshot_ ? head_score_ : body_score_;
        info.position = owner->GetTranslate();
        on_killed_(info);
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
