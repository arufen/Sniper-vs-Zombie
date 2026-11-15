#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>

USING_PTR(ComponentPlayerStats);

class ComponentPlayerStats : public Component
{
public:
    BP_COMPONENT_DECL(ComponentPlayerStats, u8"プレイヤーステータス");

    void Init() override;

    void Update() override;

    void GUI() override;

    //set max HP, current HP is refilled(最大HPを設定、現在HPも全回復)
    ComponentPlayerStatsPtr SetMaxHP(float max_hp);

    //set how long the player can not be hit after taking damage(ダメージ後の無敵時間を設定)
    ComponentPlayerStatsPtr SetInvincibleDuration(float seconds);

    //hit the player. returns true if the damage really went in(ダメージを与える。実際に通ったらtrue)
    bool TakeDamage(float damage);

    //heal the player, can not go over max HP(回復する。最大HPは超えない)
    void Heal(float amount);

    //draw HP bar + red flash. call this by hand at the END of scene PostDraw()(HPバーと赤フラッシュを描画。シーンのPostDraw末尾から手動で呼ぶ)
    void DrawHUD();

    float GetHP() const { return hp_; }

    float GetMaxHP() const { return max_hp_; }

    //0.0 = dead, 1.0 = full HP(0.0=死亡、1.0=満タン)
    float GetHPRatio() const;

    bool IsDead() const { return is_dead_; }

    bool IsInvincible() const { return invincible_time_ > 0.0f; }

private:
    static constexpr float FLASH_DURATION  = 0.4f;    //red flash length in seconds(赤フラッシュの長さ・秒)
    static constexpr int   FLASH_MAX_ALPHA = 110;     //red flash strongest alpha(赤フラッシュの最大アルファ)

    float max_hp_ = 100.0f;     //max HP(最大HP)
    float hp_     = max_hp_;    //current HP(現在のHP)

    float invincible_duration_ =
        1.0f;                         //no damage time after a hit, stops 1 overlap from hitting every frame(被弾後の無敵時間。重なりで毎フレーム当たるのを防ぐ)
    float invincible_time_ = 0.0f;    //invincible time left(無敵の残り時間)

    float flash_time_ = 0.0f;    //red flash time left(赤フラッシュの残り時間)

    bool is_dead_ = false;    //true after HP hits 0(HPが0になったらtrue)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //only the tuning values are saved, not the live HP(調整用の値だけ保存し、現在HPは保存しない)
    CEREAL_SAVELOAD(arc, ver)
    {
        arc(CEREAL_NVP(max_hp_), CEREAL_NVP(invincible_duration_));

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentPlayerStats, 1);
