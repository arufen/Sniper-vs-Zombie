#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <functional>
#include <memory>
#include <vector>

USING_PTR(ComponentStalkerJump);

//after the ground path is done, jump through a list of landing points (ground -> roof -> tower)(地上の経路が終わったら、着地点リストを順にジャンプする。地上→屋根→塔)
class ComponentStalkerJump : public Component
{
public:
    BP_COMPONENT_DECL(ComponentStalkerJump, u8"ストーカージャンプ");

    enum class State
    {
        Waiting,    //walking the ground path, not jumping yet(地上の経路を歩いている。まだ跳ばない)
        Crouch,     //standing still before a jump, the player's chance to shoot(跳ぶ前に止まる。プレイヤーが撃つチャンス)
        Jumping,    //in the air on the arc(放物線で空中にいる)
        Done,       //all jumps finished(全ジャンプ終了)
    };

    void Init() override;

    void Update() override;

    void GUI() override;

    //landing points in order, e.g. {roof, tower}(着地点を順番に。例: {屋根, 塔})
    ComponentStalkerJumpPtr SetJumpPoints(const std::vector<float3>& points);

    //the capsule that uses gravity. gravity is turned off while in the air(重力を使うカプセル。空中では重力を切る)
    ComponentStalkerJumpPtr SetFeetCollision(const std::shared_ptr<ComponentCollision>& feet);

    //seconds in the air per jump(1回のジャンプの滞空時間・秒)
    ComponentStalkerJumpPtr SetJumpDuration(float seconds);

    //extra height at the top of the arc(放物線の頂点での追加の高さ)
    ComponentStalkerJumpPtr SetJumpHeight(float height);

    //seconds standing still before each jump(各ジャンプ前に止まる時間・秒)
    ComponentStalkerJumpPtr SetCrouchTime(float seconds);

    //called once when the last jump lands. the Object uses this to start the attack(最後のジャンプ着地時に1回呼ばれる。Objectが攻撃開始に使う)
    ComponentStalkerJumpPtr SetOnFinished(std::function<void()> on_finished);

    State GetState() const { return state_; }

    bool IsInAir() const { return state_ == State::Jumping; }

private:
    //position on the jump arc. t = 0 at start, t = 1 at end(ジャンプの放物線上の位置。t=0で開始、t=1で終了)
    float3 ArcPosition(const float3& start, const float3& end, float t) const;

    void StartCrouch();
    void StartJump();
    void Land();

    //gravity on/off for the feet capsule(足カプセルの重力ON/OFF)
    void SetFeetGravity(bool on);

    std::vector<float3> jump_points_;           //landing points(着地点)
    size_t              jump_index_  = 0;       //landing point we jump to next(次に跳ぶ着地点番号)
    float3              start_       = {};      //where the current jump started(今のジャンプの開始位置)
    float               t_           = 0.0f;    //jump progress 0..1(ジャンプの進み具合 0〜1)
    float               crouch_left_ = 0.0f;    //crouch time left(しゃがみの残り時間)

    float jump_duration_ = 0.9f;     //seconds in the air(滞空時間)
    float jump_height_   = 40.0f;    //arc top height(放物線の頂点の高さ)
    float crouch_time_   = 1.0f;     //seconds before a jump(跳ぶ前の待ち時間)

    State state_ = State::Waiting;

    std::weak_ptr<ComponentCollision> feet_collision_;    //weak so this does not keep the capsule alive(カプセルの寿命を握らないようweak)

    std::function<void()> on_finished_;    //(全ジャンプ終了時コールバック)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    //only the tuning numbers are saved, not the points(調整用の数値だけ保存し、着地点は保存しない)
    CEREAL_SAVELOAD(arc, ver)
    {
        arc(CEREAL_NVP(jump_duration_), CEREAL_NVP(jump_height_), CEREAL_NVP(crouch_time_));

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentStalkerJump, 1);
