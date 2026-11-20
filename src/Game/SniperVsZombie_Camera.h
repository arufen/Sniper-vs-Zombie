//---------------------------------------------------------------------------
//!	@file	SniperVsZombie_Camera.h
//! @brief	SniperVsZombie_Camera
//---------------------------------------------------------------------------
#pragma once
#include <System/Scene.h>
#include "StageConfig.h"

namespace SniperVsZombie {
USING_PTR(Camera);
class Camera : public Object
{
public:
    BP_OBJECT_DECL(Camera, "SniperVsZombie::Camera");

    void Shoot();

    //! @brief 初期化
    //! @return 初期化終了
    bool Init() override;
    void Update() override;
    void Exit() override;

    //1.0 = just fired, 0.0 = ready to shoot. for the reload bar(1.0=撃った直後、0.0=撃てる。装填バー用)
    float GetReloadRatio() const;

    //no shot for at least this long, e.g. so the click on RESUME does not fire(最低この時間は撃てない。再開ボタンのクリックで撃たないように)
    void DelayNextShot(float seconds);

private:
    float shootIntervalTime = Config::SHOT_INTERVAL;    //from StageConfig.h(StageConfig.hから)
    float shootCurrentTime  = shootIntervalTime;

    float gun_kick_ = 0.0f;    //1 = gun fully pushed back by the shot, 0 = at rest(1=撃った反動で銃が一番後ろ、0=元の位置)

    int shootSoundHandle_ = -1;    //!< sniper shot SFX, loaded in Init()
};
}    // namespace SniperVsZombie
