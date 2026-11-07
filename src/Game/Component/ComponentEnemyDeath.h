#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <functional>
#include <memory>

USING_PTR(ComponentEnemyDeath);

class ComponentEnemyDeath : public Component
{
public:
    BP_COMPONENT_DECL(ComponentEnemyDeath, u8"敵の死亡処理");

    void Init() override;

    void Update() override;

    void GUI() override;

    //which capsule is the head, so we can tell a headshot(どのカプセルが頭か。ヘッドショット判定用)
    ComponentEnemyDeathPtr SetHeadCollision(const std::shared_ptr<ComponentCollision>& head);

    //points for a kill, body and headshot(キルのスコア。胴体とヘッドショット)
    ComponentEnemyDeathPtr SetScore(int body_score, int head_score);

    //how long the dead body stays, in seconds(死体が残る時間・秒)
    ComponentEnemyDeathPtr SetCorpseTime(float seconds);

    //called once when it dies. the Object uses this to remove its own behavior components(死んだ時に1回呼ばれる。Objectが自分の行動コンポーネントを外すのに使う)
    ComponentEnemyDeathPtr SetOnDie(std::function<void()> on_die);

    //call this from the Object's OnHit when a bullet hits(弾が当たった時にObjectのOnHitから呼ぶ)
    void OnBulletHit(const ComponentCollision::HitInfo& hit_info);

    bool IsDead() const { return is_dead_; }

    bool WasHeadshot() const { return was_headshot_; }

    //true when the dead body time is over, the Object should Release itself(死体の時間が終わったらtrue。Objectが自分をReleaseする)
    bool IsCorpseExpired() const { return is_dead_ && corpse_time_ < 0.0f; }

private:
    void Die();

    std::weak_ptr<ComponentCollision> head_collision_;    //weak so this does not keep the capsule alive(カプセルの寿命を握らないようweak)

    std::function<void()> on_die_;    //(死亡時コールバック)

    bool is_dead_      = false;    //(死亡済みか)
    bool was_headshot_ = false;    //last hit was the head(最後の被弾が頭か)

    int body_score_ = 1;    //(胴体キルのスコア)
    int head_score_ = 2;    //(ヘッドショットキルのスコア)

    float corpse_time_ = 3.0f;    //seconds until the body is removed(死体が消えるまでの秒数)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    CEREAL_SAVELOAD(arc, ver)
    {
        arc(CEREAL_NVP(body_score_), CEREAL_NVP(head_score_), CEREAL_NVP(corpse_time_));

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentEnemyDeath, 1);
