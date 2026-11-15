#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <functional>
#include <memory>
#include <vector>

USING_PTR(ComponentStalkerJump);

//after the ground path is done, jump through a list of landing points (ground -> roof -> tower)(地上の経路が終わったら、着地点リストを順にジャンプする。地上→屋根→塔)
//the "jump" anim is the clock: the arc starts and ends at fixed anim times, so body and anim always match(「jump」アニメが時計。放物線はアニメの決まった時間で始まり終わるので、体とアニメが必ず一致する)
class ComponentStalkerJump : public Component
{
public:
    BP_COMPONENT_DECL(ComponentStalkerJump, u8"ストーカージャンプ");

    enum class State
    {
        Waiting,    //walking the ground path, not jumping yet(地上の経路を歩いている。まだ跳ばない)
        Crouch,     //anim wind-up before take-off, the player's chance to shoot(離陸前のアニメの溜め。プレイヤーが撃つチャンス)
        Jumping,    //in the air on the arc(放物線で空中にいる)
        Recover,    //landed, the anim is still standing up(着地済み。アニメはまだ起き上がり中)
        Done,       //all jumps finished(全ジャンプ終了)
    };

    void Init() override;

    void Update() override;

    void GUI() override;

    //landing points in order, e.g. {roof, tower}(着地点を順番に。例: {屋根, 塔})
    ComponentStalkerJumpPtr SetJumpPoints(const std::vector<float3>& points);

    //the capsule that uses gravity. gravity is turned off while in the air(重力を使うカプセル。空中では重力を切る)
    ComponentStalkerJumpPtr SetFeetCollision(const std::shared_ptr<ComponentCollision>& feet);

    //anim times in real seconds (GetAnimationTime), at the speed set in ZombieBody(アニメ時間・実秒。ZombieBodyで設定した速度での値)
    //takeoff = feet leave the ground, land = feet touch down, length = anim end(takeoff=足が離れる、land=着地、length=アニメ終了)
    ComponentStalkerJumpPtr SetAnimTiming(float takeoff, float land, float length);

    //extra height at the top of the arc(放物線の頂点での追加の高さ)
    ComponentStalkerJumpPtr SetJumpHeight(float height);

    //called once when the last jump lands. the Object uses this to start the attack(最後のジャンプ着地時に1回呼ばれる。Objectが攻撃開始に使う)
    ComponentStalkerJumpPtr SetOnFinished(std::function<void()> on_finished);

    State GetState() const { return state_; }

    bool IsInAir() const { return state_ == State::Jumping; }

private:
    //position on the jump arc. t = 0 at start, t = 1 at end(ジャンプの放物線上の位置。t=0で開始、t=1で終了)
    float3 ArcPosition(const float3& start, const float3& end, float t) const;

    //seconds since the jump anim started, -1 if it is not playing(jumpアニメ開始からの秒数。再生中でなければ-1)
    float GetJumpAnimTime() const;

    void StartCrouch();
    void StartJump();
    void Land();
    void FinishRecover();

    //gravity on/off for the feet capsule(足カプセルの重力ON/OFF)
    void SetFeetGravity(bool on);

    std::vector<float3> jump_points_;          //landing points(着地点)
    size_t              jump_index_ = 0;       //landing point we jump to next(次に跳ぶ着地点番号)
    float3              start_      = {};      //where the current jump started(今のジャンプの開始位置)
    float               t_          = 0.0f;    //arc progress 0..1, shown in GUI(放物線の進み具合 0〜1。GUI表示用)

    //measured at jump anim speed 1.5. change ZombieBody's speed = re-measure these(jumpアニメ速度1.5で計測。ZombieBodyの速度を変えたら測り直す)
    float takeoff_time_ = 0.36f;    //feet leave the ground(足が地面を離れる)
    float land_time_    = 1.26f;    //feet touch down(着地)
    float anim_length_  = 2.0f;     //anim end(アニメ終了)
    float jump_height_  = 40.0f;    //arc top height(放物線の頂点の高さ)

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
        arc(CEREAL_NVP(takeoff_time_), CEREAL_NVP(land_time_), CEREAL_NVP(anim_length_), CEREAL_NVP(jump_height_));

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentStalkerJump, 2);
