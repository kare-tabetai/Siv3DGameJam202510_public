# MouseEffectManager設計書

## 概要

マウスクリック時の視覚的・聴覚的フィードバックを管理するクラス。クリック位置に広がる輪のエフェクトを描画し、クリック音を再生する。

## 目的・スコープ

- マウスクリック時の視覚エフェクト（広がる輪）を描画・管理する
- クリック時のSE再生を管理する
- ゲーム全体で統一されたクリックフィードバックを提供する
- モノステートパターンにより、どこからでもアクセス可能にする
- UI操作のフィードバック以外（例：3D空間でのオブジェクトインタラクション）は対象外

## ほかのクラスとの関係

- `SoundManager` - クリック音の再生をこのクラスに委譲する
- すべてのPhase系クラス - 毎フレームのUpdate/Draw時に本クラスのメソッドを呼び出す

## このクラスが継承するクラス

なし

## このクラスのコンストラクタ

デフォルトコンストラクタのみ

## このクラスのデストラクタ

デフォルトデストラクタのみ

## このクラスに含まれるデータメンバ

- `static inline Array<ClickEffect> activeEffects_` - 現在再生中のエフェクトの配列（初期値空配列）
- `static inline bool isInitialized_ = false` - 初期化済みかどうかのフラグ（初期値false）
- `static inline bool isSoundActive_ = true` - クリック音の有効/無効フラグ（初期値true）

### 内部構造体: ClickEffect

- `Vec2 position` - クリック位置の座標
- `Stopwatch timer` - エフェクト開始からの経過時間を計測
- `double duration` - エフェクトの持続時間（秒）
- `ColorF color` - エフェクトの色
- `double maxRadius` - 輪の最大半径（ピクセル）

## このクラスに含まれる定数

- `static constexpr double kEffectDuration = 0.5` - エフェクトの持続時間（秒）
- `static constexpr double kMaxRadius = 40.0` - 輪の最大半径（ピクセル）
- `static constexpr double kLineThickness = 3.0` - 輪の線の太さ（ピクセル）
- `static constexpr ColorF kEffectColor{0.8, 0.8, 1.0, 1.0}` - エフェクトの色（明るい青白）

## このクラスに含まれるpublicメソッド

- `static void Initialize()` - isInitialized_がtrueなら早期リターン。activeEffects_.clear()、isInitialized_=trueで初期化。ゲーム開始時に1度だけ呼び出す
- `static void Update()` - isInitialized_がfalseなら早期リターン。MouseL.down()でクリック判定、trueならAddEffect(Cursor::Pos())とPlayClickSound()呼び出し。activeEffects_.remove_if()で持続時間(effect.timer.sF() >= effect.duration)を超えたエフェクトを削除
- `static void Draw()` - isInitialized_がfalseなら早期リターン。activeEffects_をループしてDrawEffect(effect)を呼び出し、すべてのアクティブなエフェクトを描画
- `static void Clear()` - isInitialized_がfalseなら早期リターン。activeEffects_.clear()ですべてのエフェクトをクリア。フェーズ遷移時などに使用
- `static void SetActiveSound(bool active)` - isSoundActive_=activeでクリック音の有効/無効を設定

## このクラスに含まれるprivateメソッド

- `static void AddEffect(const Vec2& position)` - ClickEffect構造体を生成（position設定、timer.start()、duration=kEffectDuration、color=kEffectColor、maxRadius=kMaxRadius）、activeEffects_.push_back(effect)で追加
- `static void DrawEffect(const ClickEffect& effect)` - t = effect.timer.sF() / effect.duration計算、progress = TweenUtil::EaseOut(t)で進行度取得。radius = progress * effect.maxRadius、alpha = (1.0 - progress) * effect.color.aで半径とアルファ計算。Circle{effect.position, radius}.drawFrame(kLineThickness, drawColor)で輪を描画
- `static void PlayClickSound()` - isSoundActive_がfalseなら早期リターン。SoundManager::PlaySE(U"se_click")でクリック音を再生

## このクラスで参照するアセットの情報

- SE: `Asset/PC-Mouse05-1.mp3` - クリック音（SoundManager経由で再生）

## このクラスが参照する仕様書の項目

- 基本操作
  - マウス左クリックでボタンをクリック
  - メッセージウィンドウが表示されているときに画面内のどこかをクリックすることで、メッセージを進める

## イメージ図

## このクラスが使用されるフェーズ

- すべてのフェーズで使用される共通ユーティリティ

## 特記事項・メモ

- モノステートパターンで実装する（すべてのメンバとメソッドがstatic）
- `Update()`メソッド内で`MouseL.down()`を使用してクリック判定を自動実施
- クリック判定時に自動的に`Cursor::Pos()`からマウス位置を取得
- エフェクトの描画には`Circle`の`drawFrame`メソッドを使用
- エフェクトのアニメーションには`TweenUtil::easeOut`を使用して自然な減衰を表現
- 複数のエフェクトを同時に再生可能（配列で管理）
- クリック音の再生は`SoundManager::PlaySE("click")`を呼び出す
- 60FPS想定のため、Update/Drawは毎フレーム呼ばれることを前提とする
- Initialize呼び出し前の使用を防ぐため、各メソッド内でisInitialized_をチェックする
- 外部から`OnClick`を呼び出す必要がなく、`Update()`と`Draw()`のみ毎フレーム呼び出す
