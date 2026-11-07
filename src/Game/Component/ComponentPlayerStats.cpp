#include <Game/Component/ComponentPlayerStats.h>
#include <algorithm>

void ComponentPlayerStats::Init()
{
    __super::Init();

    SetName<Component>("PlayerStats");

    //always start the stage with full HP(ステージ開始時は必ず全回復)
    hp_              = max_hp_;
    invincible_time_ = 0.0f;
    flash_time_      = 0.0f;
    is_dead_         = false;
}

void ComponentPlayerStats::Update()
{
    __super::Update();

    //cast to float so std::max gets the same type on both sides(std::maxの両側をfloatに揃えるためキャスト)
    const float dt = static_cast<float>(GetDeltaTime());

    //count the timers down(タイマーを減らす)
    if(invincible_time_ > 0.0f)
        invincible_time_ = std::max(0.0f, invincible_time_ - dt);

    if(flash_time_ > 0.0f)
        flash_time_ = std::max(0.0f, flash_time_ - dt);
}

ComponentPlayerStatsPtr ComponentPlayerStats::SetMaxHP(float max_hp)
{
    max_hp_ = std::max(1.0f, max_hp);    //never 0, or the HP ratio divides by 0(0だと割合計算がゼロ除算になる)
    hp_     = max_hp_;
    return std::dynamic_pointer_cast<ComponentPlayerStats>(shared_from_this());
}

ComponentPlayerStatsPtr ComponentPlayerStats::SetInvincibleDuration(float seconds)
{
    invincible_duration_ = std::max(0.0f, seconds);
    return std::dynamic_pointer_cast<ComponentPlayerStats>(shared_from_this());
}

bool ComponentPlayerStats::TakeDamage(float damage)
{
    //ignore if already dead, still invincible, or not real damage(死亡中・無敵中・ダメージ無しは無視)
    if(is_dead_ || IsInvincible() || damage <= 0.0f)
        return false;

    hp_ = std::max(0.0f, hp_ - damage);

    invincible_time_ = invincible_duration_;
    flash_time_      = FLASH_DURATION;

    if(hp_ <= 0.0f)
        is_dead_ = true;

    return true;
}

void ComponentPlayerStats::Heal(float amount)
{
    //dead player can not be healed(死んだプレイヤーは回復できない)
    if(is_dead_ || amount <= 0.0f)
        return;

    hp_ = std::min(max_hp_, hp_ + amount);
}

float ComponentPlayerStats::GetHPRatio() const
{
    return std::clamp(hp_ / max_hp_, 0.0f, 1.0f);
}

void ComponentPlayerStats::DrawHUD()
{
    //RED FLASH when hit, fades out(被弾時の赤フラッシュ、だんだん消える)
    if(flash_time_ > 0.0f) {
        int alpha = static_cast<int>(FLASH_MAX_ALPHA * (flash_time_ / FLASH_DURATION));

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
        DrawBox(0, 0, WINDOW_W, WINDOW_H, GetColor(255, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    //HP BAR at the bottom left(左下のHPバー)
    const float x1 = 10.0f;
    const float y1 = static_cast<float>(WINDOW_H) - 60.0f;
    const float x2 = 260.0f;
    const float y2 = static_cast<float>(WINDOW_H) - 30.0f;

    const float offset = 5.0f;    //inward offset, same as the gate bar(内側の余白。ゲートバーと同じ)

    float ratio = GetHPRatio();

    //outline(枠)
    DrawBoxAA(x1, y1, x2, y2, GetColor(0, 0, 0), 1);

    //bar color goes green -> red when HP is low(HPが減ると緑から赤へ)
    int color = GetColor(static_cast<int>(255.0f * (1.0f - ratio)), static_cast<int>(255.0f * ratio), 0);

    float innerMaxWidth = (x2 - offset) - (x1 + offset);
    float innerX2       = (x1 + offset) + innerMaxWidth * ratio;
    DrawBoxAA(x1 + offset, y1 + offset, innerX2, y2 - offset, color, 1);

    //text above the bar(バーの上のテキスト)
    DrawFormatString(10, static_cast<int>(y1) - 24, GetColor(255, 255, 255), "プレイヤー HP: %d / %d", static_cast<int>(hp_), static_cast<int>(max_hp_));
}

void ComponentPlayerStats::GUI()
{
    __super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Player Stats")) {
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable))
                SetStatus(StatusBit::Enable, enable);

            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            ImGui::Text(u8"HP: %.0f / %.0f", hp_, max_hp_);
            ImGui::Text(u8"死亡: %s", is_dead_ ? "true" : "false");

            ImGui::DragFloat(u8"最大HP", &max_hp_, 1.0f, 1.0f, 9999.0f);
            ImGui::DragFloat(u8"無敵時間(秒)", &invincible_duration_, 0.05f, 0.0f, 10.0f);

            //test buttons, so you can check the HP bar without any enemy(敵なしでHPバーを確認できるテスト用ボタン)
            if(ImGui::Button(u8"テスト: 10ダメージ"))
                TakeDamage(10.0f);
            ImGui::SameLine();
            if(ImGui::Button(u8"テスト: 全回復"))
                Heal(max_hp_);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentPlayerStats)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentPlayerStats)
