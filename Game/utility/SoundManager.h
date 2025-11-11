// SoundManager.h
// ゲーム内で使用するBGMとSE(効果音)を一元管理するユーティリティクラス

#pragma once
#include <Siv3D.hpp>

#include "Game/utility/DebugUtil.h"

// ゲーム内で使用するBGMとSE(効果音)を一元管理するユーティリティクラス
// モノステートパターンで実装され、どこからでもアクセス可能
class SoundManager {
  public:
  // サウンドアセットを読み込み、各テーブルに登録する初期化処理
  static void Initialize() {
    if (isInitialized_) {
      return;
    }

    LoadBGMAssets();
    LoadSEAssets();

    isInitialized_ = true;
  }

  // クロスフェード処理を更新する。毎フレーム呼び出す必要がある
  static void Update(double deltaTime) {
    if (!isInitialized_) {
      return;
    }

    UpdateCrossFade(deltaTime);
  }

  // 指定したキーのBGMを再生。既にBGMが再生中の場合はクロスフェードで切り替える
  static void PlayBGM(const String& key, double volume = kDefaultBGMVolume) {
    if (!isInitialized_) {
      DebugUtil::Console << U"SoundManager: 初期化されていません。";
      return;
    }

    if (!bgmTable_.contains(key)) {
      DebugUtil::Console << U"SoundManager: BGMキー '" << key << U"' が見つかりません。";
      return;
    }

    // 同じBGMが既に再生中の場合は何もしない
    if (currentBGMKey_.has_value() && currentBGMKey_ == key) {
      return;
    }

    Audio& newBGM = bgmTable_[key];

    // 既にBGMが再生中の場合はクロスフェード開始
    if (currentBGM_.has_value() && currentBGM_->isPlaying()) {
      fadingOutBGM_ = currentBGM_;
      fadeTimer_ = 0.0;
    }

    currentBGM_ = newBGM;
    currentBGMKey_ = key;
    currentBGM_->setVolume(0.0);
    currentBGM_->setLoop(true);
    currentBGM_->play();

    // クロスフェードがない場合は即座に音量設定
    if (!fadingOutBGM_.has_value()) {
      currentBGM_->setVolume(volume);
    }
  }

  // 現在再生中のBGMをフェードアウトして停止する
  static void StopBGM(double fadeDuration = kFadeDuration) {
    if (!isInitialized_) {
      return;
    }

    if (currentBGM_.has_value() && currentBGM_->isPlaying()) {
      fadingOutBGM_ = currentBGM_;
      currentBGM_.reset();
      currentBGMKey_.reset();
      fadeTimer_ = 0.0;
    }
  }

  // 指定したキーのSEを再生する
  static void PlaySE(const String& key, double volume = kDefaultSEVolume) {
    if (!isInitialized_) {
      DebugUtil::Console << U"SoundManager: 初期化されていません。";
      return;
    }

    if (!seTable_.contains(key)) {
      DebugUtil::Console << U"SoundManager: SEキー '" << key << U"' が見つかりません。";
      return;
    }

    seTable_[key].playOneShot(volume);
  }

  // BGMが再生中かどうかを返す
  [[nodiscard]] static bool IsBGMPlaying() {
    if (!isInitialized_) {
      return false;
    }

    return currentBGM_.has_value() && currentBGM_->isPlaying();
  }

  // 指定したキーのBGMが現在再生中かどうかを返す
  [[nodiscard]] static bool IsBGMPlaying(const String& key) {
    if (!isInitialized_) {
      return false;
    }

    return currentBGMKey_.has_value() && currentBGMKey_ == key &&
           currentBGM_.has_value() && currentBGM_->isPlaying();
  }

  // 現在再生中のBGMの音量を設定する
  static void SetBGMVolume(double volume) {
    if (!isInitialized_) {
      return;
    }

    if (currentBGM_.has_value()) {
      currentBGM_->setVolume(volume);
    }
  }

  // SE再生時のデフォルト音量を設定する
  static void SetSEVolume(double volume) {
    // SEのデフォルト音量は実装上は使用しないが、
    // 将来的な拡張のため空実装を残す
    (void)volume;
  }

  // マスター音量を設定する
  static void SetMasterVolume(double volume) {
    GlobalAudio::SetVolume(volume);
  }

  // BGMクロスフェードの所要時間(秒)
  static constexpr double kFadeDuration = 2.0;

  // デフォルトBGM音量
  static constexpr double kDefaultBGMVolume = 0.5;

  // デフォルトSE音量
  static constexpr double kDefaultSEVolume = 0.7;

  // サウンドアセットの識別キーとファイルパスの対応表
  static inline const Array<std::pair<String, String>> kSoundTable = {
    // BGM
    {U"bgm_badend",   U"Asset/BGM/1081.ogg"                                },// G線上のアリア
    {U"bgm_clear",   U"Asset/BGM/1037.ogg"                                },// ハンガリー舞曲
    {U"bgm_amusement_park",   U"Asset/BGM/441.ogg"                                },// 花のワルツ
    {U"bgm_room_music",   U"Asset/BGM/4132.ogg"                                },// 月の光
    {U"bgm_gameover", U"Asset/BGM/White_Noise01-3(Loop).mp3"               },
    {U"bgm_room",     U"Asset/BGM/quiet-room-1.mp3"                        },
    {U"bgm_factory",  U"Asset/BGM/factory_exh_fan.wav"                     },
    // SE
    {U"se_click",     U"Asset/PC-Mouse05-1.mp3"                        },
    {U"se_keyboard1", U"Asset/keyboard/typing_mechanical_single_1.mp3" },
    {U"se_keyboard2", U"Asset/keyboard/typing_mechanical_single_2.mp3" },
    {U"se_keyboard3", U"Asset/keyboard/typing_mechanical_single_3.mp3" },
    {U"se_keyboard4", U"Asset/keyboard/typing_panta_graph_single_1.mp3"},
    {U"se_wrench",    U"Asset/wrench-02-101112.mp3"                    },
    {U"se_message",   U"Asset/piron.wav"                               }
  };

  private:
  // BGMアセットをファイルから読み込み、bgmTable_に登録する
  static void LoadBGMAssets() {
    for (const auto& [key, path] : kSoundTable) {
      // BGMはキーが"bgm_"で始まるものとする
      if (key.starts_with(U"bgm_")) {
        Audio audio{Audio::Stream, path, Loop::Yes};
        if (!audio) {
          DebugUtil::Console << U"SoundManager: BGM '" << path << U"' の読み込みに失敗しました。";
        }
        bgmTable_[key] = audio;
      }
    }
  }

  // SEアセットをファイルから読み込み、seTable_に登録する
  static void LoadSEAssets() {
    for (const auto& [key, path] : kSoundTable) {
      // SEはキーが"se_"で始まるものとする
      if (key.starts_with(U"se_")) {
        Audio audio{path};
        if (!audio) {
          DebugUtil::Console << U"SoundManager: SE '" << path << U"' の読み込みに失敗しました。";
        }
        seTable_[key] = audio;
      }
    }
  }

  // クロスフェード処理を更新する内部処理
  static void UpdateCrossFade(double deltaTime) {
    // フェードアウト中のBGMがない場合は何もしない
    if (!fadingOutBGM_.has_value()) {
      return;
    }

    fadeTimer_ += deltaTime;
    const double progress = Math::Min(fadeTimer_ / kFadeDuration, 1.0);

    // フェードアウト中のBGMの音量を減少
    if (fadingOutBGM_->isPlaying()) {
      const double fadeOutVolume = (1.0 - progress) * kDefaultBGMVolume;
      fadingOutBGM_->setVolume(fadeOutVolume);
    }

    // 新しいBGMの音量を増加
    if (currentBGM_.has_value() && currentBGM_->isPlaying()) {
      const double fadeInVolume = progress * kDefaultBGMVolume;
      currentBGM_->setVolume(fadeInVolume);
    }

    // フェード完了時の処理
    if (progress >= 1.0) {
      if (fadingOutBGM_->isPlaying()) {
        fadingOutBGM_->stop();
      }
      fadingOutBGM_.reset();
      fadeTimer_ = 0.0;
    }
  }

  // BGMのキーと音声データの対応表
  static inline HashTable<String, Audio> bgmTable_;

  // SEのキーと音声データの対応表
  static inline HashTable<String, Audio> seTable_;

  // 現在再生中のBGM
  static inline Optional<Audio> currentBGM_;

  // フェードアウト中の前のBGM
  static inline Optional<Audio> fadingOutBGM_;

  // クロスフェード用のタイマー
  static inline double fadeTimer_ = 0.0;

  // 現在再生中のBGMのキー
  static inline Optional<String> currentBGMKey_;

  // 初期化済みかどうかのフラグ
  static inline bool isInitialized_ = false;
};
