#pragma once
#include <System/Scene.h>
#include <System/Component/Component.h>
#include <string>
#include <string_view>

USING_PTR(ComponentRootMotionLock);

//stops a non-in-place anim from moving the body. code moves the Object, the anim only moves the limbs(インプレースでないアニメが体を動かさないようにする。Objectはコードが動かし、アニメは手足だけ動かす)
//it pins the root bone's translation to its rest pose, only while the chosen anim plays(指定アニメの再生中だけ、ルートボーンの移動を初期姿勢に固定する)
class ComponentRootMotionLock : public Component
{
public:
    BP_COMPONENT_DECL(ComponentRootMotionLock, u8"ルートモーション固定");

    void Init() override;

    //runs after ComponentModel::Update() set this frame's anim pose(ComponentModel::Updateがこのフレームのポーズを決めた後に実行)
    void PostUpdate() override;

    void Exit() override;

    void GUI() override;

    //root bone name, default "mixamorig:Hips"(ルートボーン名。デフォルトは"mixamorig:Hips")
    ComponentRootMotionLockPtr SetFrameName(std::string_view name);

    //lock only while this anim plays, e.g. "jump". empty = always lock(このアニメ再生中だけ固定。例:"jump"。空なら常に固定)
    ComponentRootMotionLockPtr SetAnimationName(std::string_view name);

    //which translation axes to pin(固定する移動軸)
    ComponentRootMotionLockPtr SetLockAxes(bool x, bool y, bool z);

    bool IsLocking() const { return is_locking_; }

private:
    //give the bone back to the animation(ボーンをアニメに返す)
    void Unlock();

    //model handle, or -1 when the model is not ready(モデルハンドル。未準備なら-1)
    int FindModelHandle() const;

    std::string frame_name_     = "mixamorig:Hips";    //root bone(ルートボーン)
    std::string animation_name_ = "jump";              //anim to lock(固定するアニメ)

    bool lock_x_ = true;     //forward/side travel, the code arc does this(前後左右の移動。コードの放物線が担当)
    bool lock_y_ = false;    //up/down. free = keeps the crouch dip, but adds to the arc height(上下。自由だとしゃがみの沈みが残るが、放物線の高さに足される)
    bool lock_z_ = true;     //forward/side travel(前後左右の移動)

    int  frame_index_ = -1;       //cached bone index, -1 = not found yet(ボーン番号のキャッシュ。-1は未検索)
    bool is_locking_  = false;    //true while the user matrix is set(ユーザー行列を設定中ならtrue)

    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{

    CEREAL_SAVELOAD(arc, ver)
    {
        arc(CEREAL_NVP(frame_name_), CEREAL_NVP(animation_name_), CEREAL_NVP(lock_x_), CEREAL_NVP(lock_y_), CEREAL_NVP(lock_z_));

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }
};

CEREAL_CLASS_VERSION(ComponentRootMotionLock, 1);
