# SisterMessagePhase設計書

## 概要

妹とのLINE風チャットを通じて交流するフェーズを管理するクラス。プレイヤーの入力を受け取り、ローカルLLMを用いて妹AIの返信を生成し、精神力を回復させる。5つの状態(Chatting/BirthDayMessageInterval/WaitWindowMessage/DisplayingWindowMessage/FadeoutWait)を持ち、メッセージ受信から次フェーズへの遷移まで管理する。

## 目的・スコープ

- 妹とのメッセージのやり取りを管理する
- ローカルLLMを用いた妹AIの返信生成を制御する(SisterMessageUI経由)
- 妹の返信受信時に精神力を回復させる(3～7ランダム)
- 最終日前日の誕生日メッセージの特殊処理を行う
- メッセージ受信後の遅延処理とウィンドウメッセージ表示を管理する
- 暗転による次フェーズ(Work)への遷移を制御する
- LINE風UIのアクティブ/非アクティブを制御する

## ほかのクラスとの関係

- `iPhase` - このクラスを継承する基底クラス
- `SisterMessageUI` - `SisterMessageUIManager` が所有する `std::shared_ptr<SisterMessageUI>` を参照。フェーズは `SisterMessageUIManager::GetSisterMessageUI()` で取得した `std::weak_ptr<SisterMessageUI>` を必要時に `lock()` して利用する
- `SisterMessageUIManager` - SisterMessageUIの取得元。GetSisterMessageUI()でweak_ptrを取得
- `GameCommonData` - 精神力の回復処理、最終日前日の判定(IsBeforeFinalDay())にアクセス
- `MessageWindowUI` - フェーズ終了時の「バイトの時間だ...」メッセージ表示に使用
- `BlackOutUI` - 画面暗転によるフェーズ遷移演出に使用
- `PhaseManager` - 次フェーズ(Work)への遷移指示を行う
- `SoundManager` - 室内環境音BGM(bgm_room/bgm_room_music)の再生判定に使用
- `BackGroundManager` - 背景画像(my_room)の設定
- `MouseEffectManager` - マウスエフェクト音の無効化

## このクラスが継承するクラス

`iPhase`を継承する

## このクラスのコンストラクタ

`explicit SisterMessagePhase()` - コンストラクタ。BGMの再生判定(bgm_room/bgm_room_musicが再生中でなければbgm_roomを再生)、背景設定(my_room)、マウスエフェクト音の無効化、SisterMessageUIのアクティブ化を行う

## このクラスのデストラクタ

`~SisterMessagePhase() override` - デフォルトデストラクタ

## このクラスに含まれるデータメンバ

- `State currentState_` - フェーズの現在の状態(初期値Chatting)
- `bool transitionStarted_` - フェード遷移が開始済みかどうか(Work移行の二重呼び出し防止、初期値false)
- `Stopwatch waitTimer_` - ウィンドウ表示待機に使うタイマー

## このクラスに含まれる定数

### 状態定義

- `enum class State` - フェーズ内の状態を表す列挙型
  - `Chatting` - チャット中
  - `BirthDayMessageInterval` - 誕生日メッセージ表示待ち
  - `WaitWindowMessage` - ウィンドウメッセージ表示待ち
  - `DisplayingWindowMessage` - ウィンドウメッセージ表示中
  - `FadeoutWait` - フェーズ終了待ち

### 精神力回復関連

- `static constexpr int32 kMinMentalRecovery = 3` - 妹のメッセージによる精神力回復の最小値
- `static constexpr int32 kMaxMentalRecovery = 7` - 妹のメッセージによる精神力回復の最大値

### タイミング関連

- `static constexpr double kWindowMessageDelaySeconds = 3.0` - ウィンドウメッセージ表示までの遅延（秒）
- `static constexpr double kBirthDayMessageIntervalSeconds = 1.0` - 誕生日メッセージを表示するまでのインターバル（秒）

## このクラスに含まれるpublicメソッド

- `void update() override` - 毎フレーム呼ばれる更新処理。現在の状態(State)に応じて各ハンドラーメソッドを呼び出す
- `void draw() const override` - 毎フレーム呼ばれる描画処理。SisterMessageUIの描画はGameManagerで行われるため、フェーズ固有の描画は行わない

## このクラスに含まれるprivateメソッド

- `void handleChatting()` - Chatting状態の更新処理。checkForSisterMessage()を呼び出してメッセージ受信をチェック
- `void handleBirthDayMessageInterval()` - BirthDayMessageInterval状態の更新処理。kBirthDayMessageIntervalSeconds経過後にsetupBirthdayMessage()を呼び出し、WaitWindowMessage状態に遷移
- `void handleWaitWindowMessage()` - WaitWindowMessage状態の更新処理。kWindowMessageDelaySeconds経過後にMessageWindowUIで「...」「バイトの時間だ...」を表示し、DisplayingWindowMessage状態に遷移
- `void handleDisplayingWindowMessage()` - DisplayingWindowMessage状態の更新処理。MessageWindowUIが閉じられたら暗転を開始してFadeoutWait状態に遷移
- `void handleFadeoutWait()` - FadeoutWait状態の更新処理。暗転完了後に一度だけPhaseManager::ChangePhase(PhaseType::Work)を呼び出し、即座にBlackOutUI::FadeOut()で画面を戻す。SisterMessageUIを非アクティブ化
- `void recoverMentalPower()` - 精神力をRandom(kMinMentalRecovery, kMaxMentalRecovery)でランダムに回復させ、コンソールに回復量と現在値を出力
- `void checkForSisterMessage()` - SisterMessageUIManager経由でUIを取得し、IsMessageReceived()でメッセージ受信をチェック。受信時は精神力回復、waitTimer_再起動、最終日前日判定(GameCommonData::IsBeforeFinalDay())により次の状態(BirthDayMessageIntervalまたはWaitWindowMessage)に遷移
- `void setupBirthdayMessage()` - 誕生日メッセージ「お兄ちゃん、明日は誕生日だね！！\nお祝い送るから楽しみにしててね❤️」をSisterMessageUI::AddSisterMessage()で追加し、精神力回復を実行

## このクラスで参照するアセットの情報

- BGM: `bgm_room` / `bgm_room_music` - 室内の環境音（SoundManagerから再生判定）
- 背景: `my_room` - 自室の背景画像（BackGroundManagerで設定）
- フォント: `Asset/IBM_Plex_Sans_JP` - UI表示に使用（MessageWindowUI経由）

## このクラスが参照する仕様書の項目

- 妹メッセージフェーズ
- 精神力
- 妹について
- 誕生日メッセージ
- フェーズ遷移図

## このクラスが使用されるフェーズ

- 妹メッセージフェーズ

## 特記事項・メモ

- 5つの状態を持ち、メッセージ受信からWork遷移まで一連の流れを管理
  - Chatting: 通常のチャット中、SisterMessageUI::IsMessageReceived()でメッセージ受信を監視
  - BirthDayMessageInterval: 最終日前日のみ、1秒のインターバル後に誕生日メッセージを追加
  - WaitWindowMessage: メッセージ受信後3秒待機してからウィンドウメッセージ表示
  - DisplayingWindowMessage: 「...」「バイトの時間だ...」のウィンドウメッセージ表示中
  - FadeoutWait: 暗転完了後にWork遷移、即座にフェードアウトで画面を戻す
- 誕生日メッセージは最終日前日のみ表示(GameCommonData::IsBeforeFinalDay()で判定)
- 誕生日メッセージの内容: "お兄ちゃん、明日は誕生日だね！！\nお祝い送るから楽しみにしててね❤️"
- 精神力回復はメッセージ受信時と誕生日メッセージ表示時の2回行われる(誕生日がある場合)
- 精神力回復量はRandom(3, 7)でランダム
- LLMの応答処理は全てSisterMessageUI内のLlmChatWindowが担当
- SisterMessageUIはSisterMessageUIManager::GetSisterMessageUI().lock()で使用毎に取得
- フェーズ開始時にSisterMessageUIをアクティブ化、終了時に非アクティブ化
- 暗転遷移の二重呼び出し防止のためtransitionStarted_フラグを使用
- SisterMessageUIの更新・描画はGameManagerで行われるため、フェーズ側では状態管理のみ
- マウスエフェクト音は無効化(MouseEffectManager::SetActiveSound(false))
- BGMは既に再生中の場合は重複再生しない(bgm_roomまたはbgm_room_musicの確認)
