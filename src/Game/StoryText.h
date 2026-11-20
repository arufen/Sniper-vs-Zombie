//---------------------------------------------------------------------------
//!	@file	StoryText.h
//! @brief	every story / radio line in one place. rewrite the story = edit only this file(全てのストーリー・無線の文を1か所に。文章の変更はこのファイルだけ)
//---------------------------------------------------------------------------
#pragma once

//IMPORTANT: plain "..." strings, NOT u8"...". MSVC stores them as Shift-JIS (code page 932) and DxLib reads Shift-JIS(重要: u8ではなく普通の"..."。MSVCはShift-JISで保存し、DxLibもShift-JISで読む)
//only use characters that exist in Shift-JIS. e.g. the long dash "—" does NOT exist (warning C4566)(Shift-JISにある文字だけ使う。例: 長いダッシュ「—」は無い(警告C4566))
namespace SniperVsZombie::Story {

//---------------------------------------------------------------------------
// INTRO(イントロ) - outbreak -> shelter -> helicopter -> your job(感染拡大→避難→ヘリ→あなたの任務)
//---------------------------------------------------------------------------
inline constexpr const char* INTRO_LINES[] = {
    "街はゾンビの大量発生に襲われた。",    //EN: The city has been overrun by a zombie outbreak.
    "",
    "生き残った市民は、ビルの中に避難している。",      //EN: The surviving civilians are sheltering inside a building.
    "救助ヘリが、屋上から一人ずつ運び出している。",    //EN: A rescue helicopter is flying them out from the roof, one by one.
    "",
    "ゾンビがビルのゲートに迫っている。",          //EN: Zombies are closing in on the building's gate.
    "あなたは狙撃手。塔の上からゲートを守れ。",    //EN: You are the sniper. Defend the gate from the tower.
    "",
    "全員が避難するまで、持ちこたえろ。",    //EN: Hold out until everyone is evacuated.
};
inline constexpr const char* INTRO_PROMPT_SKIP     = "クリックで全文表示";    //EN: Click to show all
inline constexpr const char* INTRO_PROMPT_CONTINUE = "クリックして続ける";    //EN: Click to continue

//---------------------------------------------------------------------------
// MAIN STAGE(メインステージ)
//---------------------------------------------------------------------------
inline constexpr const char* OPENING_SKIP = "クリックでスキップ";    //EN: Click to skip

//radio = shown at the bottom with the [無線] tag(無線=画面下に[無線]タグ付きで表示)
inline constexpr const char* RADIO_TAG = "[無線]";    //EN: [Radio]

inline constexpr const char* RADIO_MISSION_START  = "狙撃手、避難が始まった。ゲートを守れ。";    //EN: Sniper, the evacuation has started. Keep the gate secure.
inline constexpr const char* RADIO_OBJECTIVE      = "全員が避難するまでゲートを守り抜け！";      //EN: Defend the gate until everyone is evacuated!
inline constexpr const char* RADIO_GATE_FIRST_HIT = "ゲートが攻撃されている！突破させるな！";    //EN: The gate is taking hits! Don't let them break through!
inline constexpr const char* RADIO_GATE_HALF      = "ゲートの耐久が半分を切った！";              //EN: The gate is below half strength!
inline constexpr const char* RADIO_GATE_LOW       = "ゲートが持たない！急げ！";                  //EN: The gate won't hold much longer! Hurry!
inline constexpr const char* RADIO_STALKER        = "屋上に動きあり！狙撃位置に注意しろ！";      //EN: Movement on the rooftops! Watch your position!
inline constexpr const char* RADIO_CIVILIAN_HALF  = "市民の半分が脱出した。持ちこたえろ。";      //EN: Half the civilians are out. Keep holding.
inline constexpr const char* RADIO_CIVILIAN_ALMOST = "あと少しで全員だ。もう少しだけ耐えろ！";    //EN: Almost everyone is out. Just a little longer!
inline constexpr const char* RADIO_PLAYER_HIT      = "攻撃を受けている！近くの敵を排除しろ！";    //EN: You're under attack! Take out the enemy near you!
inline constexpr const char* RADIO_PLAYER_LOW      = "狙撃手、負傷がひどい！耐えてくれ！";        //EN: Sniper, you're badly hurt! Hang in there!

//ending lines in MainStage, right before the fade(MainStageの終了時の無線。フェード直前)
inline constexpr const char* RADIO_END_WIN    = "全員の避難が完了した。今から迎えに行く！";    //EN: All civilians are evacuated. We're coming for you!
inline constexpr const char* RADIO_END_GATE   = "ゲートが突破された！入口を守れない！";        //EN: The gate is breached! We can't hold the entrance!
inline constexpr const char* RADIO_END_SNIPER = "狙撃手、応答しろ！狙撃手！";                  //EN: Sniper, respond! Sniper!

//---------------------------------------------------------------------------
// GAME OVER(ゲームオーバー)
//---------------------------------------------------------------------------
inline constexpr const char* RADIO_HELI_PICKUP = "乗れ！ここを離れるぞ！";             //EN: Get in! We're getting out of here!
inline constexpr const char* RADIO_HELI_ESCAPE = "脱出成功。よくやった、狙撃手。";     //EN: Extraction complete. Good work, sniper.
inline constexpr const char* RADIO_GATE_LOST   = "ビルに侵入された…作戦は失敗だ。";    //EN: They're inside the building... the operation has failed.
inline constexpr const char* RADIO_SNIPER_LOST =
    "狙撃手からの応答なし…ゲートの守りが消えた。";    //EN: No response from the sniper... the gate has no defense left.

//result screen(リザルト画面)
inline constexpr const char* RESULT_WIN_EN    = "Rescue complete";
inline constexpr const char* RESULT_WIN_JP    = "救出作戦完了";    //EN: Rescue operation complete
inline constexpr const char* RESULT_LOSE_EN   = "Rescue failed";
inline constexpr const char* RESULT_GATE_JP   = "ゲートが突破された";    //EN: The gate was breached
inline constexpr const char* RESULT_SNIPER_JP = "狙撃手がやられた";      //EN: The sniper was killed
//result stats table: label on the left, number on the right(結果の数値表: 左に名前、右に数値)
inline constexpr const char* RESULT_STAT_SCORE     = "スコア";              //EN: Score
inline constexpr const char* RESULT_STAT_CIVILIANS = "救出した市民";        //EN: Civilians saved
inline constexpr const char* RESULT_STAT_KILLS     = "倒したゾンビ";        //EN: Zombies killed
inline constexpr const char* RESULT_STAT_HEADSHOT  = "ヘッドショット率";    //EN: Headshot rate
inline constexpr const char* RESULT_STAT_ACCURACY  = "命中率";              //EN: Accuracy
inline constexpr const char* RESULT_STAT_GATE      = "ゲート耐久";          //EN: Gate strength left
inline constexpr const char* RESULT_STAT_TIME      = "プレイ時間";          //EN: Play time

}    // namespace SniperVsZombie::Story
