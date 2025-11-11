# SoundManager設計書

## 概要

ゲーム内で使用するBGMとSE(効果音)を一元管理するユーティリティクラス。モノステートパターンで実装され、どこからでもアクセス可能。

## 目的・スコープ

- ゲーム内で使用するすべての音声アセットの再生・停止を管理する
- BGMのクロスフェード機能を提供する
- 文字列キーによる簡潔な再生インターフェースを提供する
- モノステートパターンにより、インスタンス化せずに利用可能
- サウンド以外のアセット管理は対象外

## ほかのクラスとの関係

- 全てのPhaseクラス - 各フェーズからBGM・SE再生を要求される
- `GameManager` - ゲーム全体のサウンド管理を担当
- 各UIクラス - ボタンクリック音などのSE再生を要求される

## このクラスが継承するクラス

なし(モノステートパターンのため継承なし)

## このクラスのコンストラクタ

デフォルトコンストラクタのみ。実際の初期化は`Initialize()`メソッドで行う。

## このクラスのデストラクタ

デフォルトデストラクタ。再生中のサウンドは自動的に停止される。

## このクラスに含まれるデータメンバ

- `static inline HashTable<String, Audio> bgmTable_` - BGMのキーと音声データの対応表（初期値空）
- `static inline HashTable<String, Audio> seTable_` - SEのキーと音声データの対応表（初期値空）
- `static inline Optional<Audio> currentBGM_` - 現在再生中のBGM（初期値nullopt）
- `static inline Optional<Audio> fadingOutBGM_` - フェードアウト中の前のBGM（初期値nullopt）
- `static inline double fadeTimer_ = 0.0` - クロスフェード用のタイマー（初期値0.0）
- `static inline Optional<String> currentBGMKey_` - 現在再生中のBGMのキー（初期値nullopt）
- `static inline bool isInitialized_ = false` - 初期化済みかどうかのフラグ（初期値false）

## このクラスに含まれる定数

- `static constexpr double kFadeDuration = 2.0` - BGMクロスフェードの所要時間(秒)
- `static constexpr double kDefaultBGMVolume = 0.5` - デフォルトBGM音量
- `static constexpr double kDefaultSEVolume = 0.7` - デフォルトSE音量
- `static inline const Array<std::pair<String, String>> kSoundTable` - サウンドアセットの識別キーとファイルパスの対応表（配列形式）。BGM: bgm_badend(G線上のアリア), bgm_clear(ハンガリー舞曲), bgm_amusement_park(花のワルツ), bgm_room_music(月の光), bgm_gameover(ホワイトノイズ), bgm_room(quiet-room), bgm_factory(factory環境音)。SE: se_click, se_keyboard1-4, se_wrench, se_message

## このクラスに含まれるpublicメソッド

- `static void Initialize()` - isInitialized_がtrueなら早期リターン。LoadBGMAssets()とLoadSEAssets()呼び出し、isInitialized_=trueに設定
- `static void Update(double deltaTime)` - isInitialized_がfalseなら早期リターン。UpdateCrossFade(deltaTime)でクロスフェード処理を更新。毎フレーム呼び出す必要がある
- `static void PlayBGM(const String& key, double volume = kDefaultBGMVolume)` - bgmTable_.contains(key)でキー存在確認。currentBGMKey_==keyなら早期リターン（同じBGM再生中）。currentBGM_がisPlaying()ならfadingOutBGM_に保存してクロスフェード開始。newBGMをsetVolume(0.0), setLoop(true), play()で再生開始。fadingOutBGM_がnulloptならsetVolume(volume)で即座に音量設定
- `static void StopBGM(double fadeDuration = kFadeDuration)` - currentBGM_がisPlaying()ならfadingOutBGM_に保存、currentBGM_.reset(), currentBGMKey_.reset(), fadeTimer_=0.0でリセット
- `static void PlaySE(const String& key, double volume = kDefaultSEVolume)` - seTable_.contains(key)でキー存在確認、seTable_[key].playOneShot(volume)で再生
- `[[nodiscard]] static bool IsBGMPlaying()` - currentBGM_.has_value() && currentBGM_->isPlaying()を返す
- `[[nodiscard]] static bool IsBGMPlaying(const String& key)` - currentBGMKey_.has_value() && currentBGMKey_==key && currentBGM_.has_value() && currentBGM_->isPlaying()を返す
- `static void SetBGMVolume(double volume)` - currentBGM_.has_value()ならcurrentBGM_->setVolume(volume)
- `static void SetSEVolume(double volume)` - 空実装（将来的な拡張のため残す）
- `static void SetMasterVolume(double volume)` - GlobalAudio::SetVolume(volume)でマスター音量設定

## このクラスに含まれるprivateメソッド

- `static void LoadBGMAssets()` - kSoundTableをループし、key.starts_with(U"bgm_")の要素をAudio{Audio::Stream, path, Loop::Yes}でロード、bgmTable_[key]に登録。ロード失敗時はConsoleログ出力
- `static void LoadSEAssets()` - kSoundTableをループし、key.starts_with(U"se_")の要素をAudio{path}でロード、seTable_[key]に登録。ロード失敗時はConsoleログ出力
- `static void UpdateCrossFade(double deltaTime)` - fadingOutBGM_.has_value()でなければ早期リターン。fadeTimer_ += deltaTime、progress = Math::Min(fadeTimer_ / kFadeDuration, 1.0)計算。fadingOutBGM_の音量を(1.0 - progress) * kDefaultBGMVolume、currentBGM_の音量をprogress * kDefaultBGMVolumeに設定。progress>=1.0でfadingOutBGM_->stop()とfadingOutBGM_.reset()

## このクラスで参照するアセットの情報

### BGM

- バッドエンド用BGM: `Asset/1081.mp3` (G線上のアリア、音質低め)
- ゲームオーバー用BGM: `Asset/White_Noise01-3(Loop).mp3` (ホワイトノイズ)
- 室内環境音: `Asset/quiet-room-1.mp3` (求職活動・妹メッセージフェーズ用)
- 工場環境音: `Asset/factory_exh_fan.mp3` (アルバイトフェーズ用)

### SE

- ボタンクリック音: `Asset/PC-Mouse05-1.mp3`
- キーボード打鍵音1: `Asset/keyboard/タイピング-メカニカル単1.mp3`
- キーボード打鍵音2: `Asset/keyboard/タイピング-メカニカル単2.mp3`
- キーボード打鍵音3: `Asset/keyboard/タイピング-メカニカル単3.mp3`
- レンチ音: `Asset/wrench-02-101112.mp3` (ボルトを締める音)
- メッセージ送信音: `Asset/piron.wav` (LINE風UI用)

## このクラスが参照する仕様書の項目

- 演出
- アセットリスト - BGM
- アセットリスト - SE
- バッドエンドフェーズ
- ゲームオーバーフェーズ
- 求職活動フェーズ
- 妹メッセージフェーズ
- アルバイトフェーズ

## このクラスが使用されるフェーズ

- 全てのフェーズ

## 特記事項・メモ

- モノステートパターンで実装し、静的メンバのみを使用する
- クロスフェード中は2つのBGMが同時に再生される(片方はフェードアウト、片方はフェードイン)
- SEは複数同時再生可能
- ゲーム開始時に`Initialize()`を必ず呼び出す必要がある
- `Update()`メソッドは`GameManager`から毎フレーム呼び出される想定
- 文字列キーは以下のような命名規則を推奨
  - BGM: `"bgm_badend"`, `"bgm_gameover"`, `"bgm_room"`, `"bgm_factory"`
  - SE: `"se_click"`, `"se_keyboard1"`, `"se_keyboard2"`, `"se_keyboard3"`, `"se_wrench"`, `"se_message"`
- 現在再生中のBGMキーを`currentBGMKey_`で保持し、同じBGMの重複再生を防ぐ
