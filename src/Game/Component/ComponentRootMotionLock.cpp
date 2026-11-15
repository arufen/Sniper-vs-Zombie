#include <Game/Component/ComponentRootMotionLock.h>
#include <System/Component/ComponentModel.h>

void ComponentRootMotionLock::Init()
{
    __super::Init();

    SetName<Component>("RootMotionLock");
}

int ComponentRootMotionLock::FindModelHandle() const
{
    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model || !model->IsValid())
        return -1;
    return model->GetModel();
}

void ComponentRootMotionLock::PostUpdate()
{
    __super::PostUpdate();

    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model || !model->IsValid())
        return;

    const int handle = model->GetModel();

    //the model loads after Init, so search the bone on first use(モデルはInitより後に読まれるので、初回使用時にボーンを探す)
    if(frame_index_ < 0) {
        frame_index_ = DxLib::MV1SearchFrame(handle, frame_name_.c_str());
        if(frame_index_ < 0)
            return;    //wrong bone name. check the GUI(ボーン名が違う。GUIで確認)
    }

    const bool want_lock = animation_name_.empty() || model->GetPlayAnimationName() == animation_name_;
    if(!want_lock) {
        Unlock();
        return;
    }

    //1. clear last frame's override so we read the real anim pose(前フレームの上書きを消して、本当のアニメポーズを読む)
    DxLib::MV1ResetFrameUserLocalMatrix(handle, frame_index_);

    //2. animated pose vs rest pose. DxLib matrices keep translation in row 3(アニメ後の姿勢と初期姿勢。DxLibの行列は3行目が移動量)
    MATRIX animated = DxLib::MV1GetFrameLocalMatrix(handle, frame_index_);
    MATRIX rest     = DxLib::MV1GetFrameBaseLocalMatrix(handle, frame_index_);

    //3. keep the anim's rotation, put back the rest translation on locked axes(アニメの回転は残し、固定軸だけ初期位置に戻す)
    if(lock_x_)
        animated.m[3][0] = rest.m[3][0];
    if(lock_y_)
        animated.m[3][1] = rest.m[3][1];
    if(lock_z_)
        animated.m[3][2] = rest.m[3][2];

    //4. write it back. it wins over the anim until it is reset(書き戻す。リセットされるまでアニメより優先される)
    DxLib::MV1SetFrameUserLocalMatrix(handle, frame_index_, animated);
    is_locking_ = true;
}

void ComponentRootMotionLock::Unlock()
{
    if(!is_locking_)
        return;

    //without this the hips freeze in the last jump pose(これが無いと腰が最後のジャンプ姿勢で固まる)
    const int handle = FindModelHandle();
    if(handle >= 0 && frame_index_ >= 0)
        DxLib::MV1ResetFrameUserLocalMatrix(handle, frame_index_);

    is_locking_ = false;
}

void ComponentRootMotionLock::Exit()
{
    Unlock();

    __super::Exit();
}

ComponentRootMotionLockPtr ComponentRootMotionLock::SetFrameName(std::string_view name)
{
    Unlock();
    frame_name_  = name;
    frame_index_ = -1;    //search again next frame(次のフレームで再検索)
    return std::dynamic_pointer_cast<ComponentRootMotionLock>(shared_from_this());
}

ComponentRootMotionLockPtr ComponentRootMotionLock::SetAnimationName(std::string_view name)
{
    animation_name_ = name;
    return std::dynamic_pointer_cast<ComponentRootMotionLock>(shared_from_this());
}

ComponentRootMotionLockPtr ComponentRootMotionLock::SetLockAxes(bool x, bool y, bool z)
{
    lock_x_ = x;
    lock_y_ = y;
    lock_z_ = z;
    return std::dynamic_pointer_cast<ComponentRootMotionLock>(shared_from_this());
}

void ComponentRootMotionLock::GUI()
{
    __super::GUI();

    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode(u8"Root Motion Lock")) {
            bool enable = GetStatus(StatusBit::Enable);
            if(ImGui::Checkbox(u8"有効", &enable)) {
                if(!enable)
                    Unlock();    //PostUpdate stops running, so give the bone back now(PostUpdateが止まるので今ボーンを返す)
                SetStatus(StatusBit::Enable, enable);
            }

            if(ImGui::Button(u8"削除"))
                GetOwner()->RemoveComponent(shared_from_this());

            ImGui::Text(u8"ボーン: %s (index %d)", frame_name_.c_str(), frame_index_);
            ImGui::Text(u8"アニメ: %s", animation_name_.empty() ? u8"(常に)" : animation_name_.c_str());
            ImGui::Text(u8"固定中: %s", is_locking_ ? "YES" : "no");

            ImGui::Checkbox(u8"X固定", &lock_x_);
            ImGui::SameLine();
            ImGui::Checkbox(u8"Y固定", &lock_y_);
            ImGui::SameLine();
            ImGui::Checkbox(u8"Z固定", &lock_z_);

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

CEREAL_REGISTER_TYPE(ComponentRootMotionLock)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentRootMotionLock)
