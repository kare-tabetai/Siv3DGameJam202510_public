// RejectionInfo.h
#pragma once
#include <Siv3D.hpp>

#include "Game/utility/FontManager.h"
#include "Game/utility/UiConst.h"

// 1つの企業からの不採用情報を保持し、描画も行うクラス
// 企業名、お祈りメッセージ、採用結果をまとめて管理し、指定された領域に描画する
class RejectionInfo {
  public:
  // ランダムな企業名とお祈りメッセージを設定するコンストラクタ(不採用)
  RejectionInfo()
      : companyName(kCompanyNames[Random(0, static_cast<int32>(kCompanyNames.size()) - 1)]),
        message(kRejectionMessages[Random(0, static_cast<int32>(kRejectionMessages.size()) - 1)]),
        isRejected(true) {}

  // デストラクタ
  ~RejectionInfo() = default;

  // 採用情報を生成する静的メソッド
  static RejectionInfo CreateRecruitmentInfo() {
    RejectionInfo info;
    info.companyName = kCompanyNames[Random(0, static_cast<int32>(kCompanyNames.size()) - 1)];
    info.message = kRecruitmentMessages[Random(0, static_cast<int32>(kRecruitmentMessages.size()) - 1)];
    info.isRejected = false;
    return info;
  }

  // 描画に関連する定数
  static constexpr int32 kRejectedLabelFontSize = 24;               // 「不採用」ラベルのフォントサイズ
  static constexpr int32 kCompanyFontSize = 20;                     // 企業名のフォントサイズ
  static constexpr int32 kMessageFontSize = 14;                     // お祈りメッセージのフォントサイズ
  static constexpr ColorF kTextColor = UiConst::kUITextColor;       // テキストの色
  static constexpr ColorF kItemBgColor = UiConst::kInputAreaColor;  // 項目の背景色（白系）
  static constexpr ColorF kRejectedLabelColor{0.85, 0.05, 0.05};    // 「不採用」ラベルの色(赤、ラベル塗りつぶし用)
  static constexpr ColorF kRecruitedLabelColor{0.05, 0.65, 0.15};   // 「採用」ラベルの色(緑、ラベル塗りつぶし用)
  static constexpr ColorF kBorderColor{0.0, 0.0, 0.0};              // 枠線の色（黒）
  static constexpr double kBorderThickness = 1.0;                   // 枠線の太さ
  static constexpr double kCornerRadius = 0.0;                      // 角丸半径（角丸なし）

  // 指定された矩形領域に不採用情報を描画する
  void Draw(const RectF& itemRect) const {
    const Font& rejectedFont = FontManager::GetFont(U"ui_medium");
    const Font& companyFont = FontManager::GetFont(U"ui_medium");
    const Font& messageFont = FontManager::GetFont(U"ui_small");

    // 項目矩形を描画（白背景 + 枠）
    itemRect.rounded(kCornerRadius).draw(kItemBgColor).drawFrame(kBorderThickness, kBorderColor);

    // ラベル用の塗りつぶし矩形
    const RectF labelRect{itemRect.x + 8.0, itemRect.y + 6.0, 92.0, itemRect.h - 12.0};
    const ColorF labelColor = isRejected ? kRejectedLabelColor : kRecruitedLabelColor;
    labelRect.rounded(2.0).draw(labelColor);

    // ラベル文字（白）を中央寄せで描画
    const String labelText = isRejected ? U"不採用" : U"採用";
    rejectedFont(labelText).drawAt(kRejectedLabelFontSize, labelRect.center(), Palette::White);

    // 企業名を描画（ラベルの右側に配置）
    const Vec2 companyPos = Vec2{labelRect.x + labelRect.w + 12.0, itemRect.y + 12.0};
    companyFont(companyName).draw(kCompanyFontSize, companyPos, kTextColor);

    // メッセージを描画（企業名の右側、1行に収めるイメージ）
    const Vec2 messagePos = Vec2{companyPos.x + 320.0, itemRect.y + 12.0};
    messageFont(message).draw(kMessageFontSize, messagePos, kTextColor);
  }

  // データメンバ
  String companyName;  // 企業名
  String message;      // 不採用の場合お祈りメッセージ(不採用通知の文章)、採用の場合、採用メッセージ
  bool isRejected;     // 採用結果

  private:
  // 企業名のリスト
  static inline const Array<String> kCompanyNames = {
    U"未来システム開発株式会社",
    U"デジタルイノベーション合同会社",
    U"AI推進株式会社",
    U"スマートフューチャーテクノロジー",
    U"量子コンピューティング研究所",
    U"クラウドシステムズ株式会社",
    U"ロボティクス産業株式会社",
    U"ブルーコードテクノロジー",
    U"インフォヴェルス",
    U"グリッドフォース",
    U"リーフコード株式会社",
    U"クオンティカ",
    U"ノヴァウェーブ",
    U"クロスパス株式会社",
    U"サイバーミューズ",
    U"ブランチソフト",
    U"ゼロフレーム",
    U"オプティクラフト",
    U"ヴィジョナリーテック",
    U"メトリカシステムズ",
    U"フュージョンコード",
    U"トリプレックスソフト",
    U"シグナテック",
    U"ネビュラスラボ",
    U"ルミナコード株式会社",
    U"セントリックス",
    U"アクシオンワークス",
    U"ハーモニックラボ",
    U"データリウム",
    U"インサイトフォージ",
    U"クラリティラボ",
    U"ネオスフィア",
    U"ファストビット株式会社",
    U"ディープ・データ・イノベーション",
    U"フュージョン・テック・スタジオ",
    U"リゾルブ・ソリューション・ラボ",
    U"スペクトル・システム・デザイン",
    U"パラダイム・シフト・ラボ",
    U"シリウス・ウェブ・クリエイト",
    U"エクリプス・テック・ワークス",
    U"ゼニス・デジタル・コンサル",
    U"オプティマル・ソフトウェア",
    U"プライム・コード・ファクトリ",
    U"エターナル・リンク・テクノ",
    U"プログレッシブ・インフォ",
    U"ダイナミック・コア・エンジン",
    U"メビウス・システム・アーキ",
    U"コスモ・ウェブ・デザイン",
    U"ニューロン・システム・ラボ",
    U"クラウド・パイロット・ワークス",
    U"ロジカル・ゲート・ソリューション",
    U"シンフォニー・IT・ラボ",
    U"アトラス・デジタル・ギルド"};

  // 採用メッセージのリスト
  static inline const Array<String> kRecruitmentMessages = {
    U"採用が決定しました。おめでとうございます。",
    U"内定が決まりました。詳細は後日ご連絡。",
    U"貴殿を採用いたします。おめでとうございます。",
    U"内定のご連絡です。続報は追って通知します。",
    U"選考の結果、採用といたしました。",
    U"厳正な選考の結果、内定です。",
    U"内定おめでとうございます。手続きは後日。",
    U"採用内定です。おめでとうございます。",
    U"採用となりました。詳細は別途ご案内。",
    U"内定が決定しました。おめでとうございます。",
    U"採用が内定しました。今後の流れは後日。",
    U"この度、採用が決定しました。",
    U"採用です。おめでとうございます。",
    U"内定です。詳細は追ってご連絡します。",
    U"採用決定のご連絡です。おめでとうございます。",
    U"貴殿の内定が決まりました。",
    U"採用となりました。おめでとうございます。",
    U"内定お知らせ。手続き案内は後日。",
    U"選考の結果、内定となりました。",
    U"採用決定。詳細は後日お知らせします。",
    U"内定が決定。おめでとうございます。",
    U"採用と決定しました。続報は後日。",
    U"採用が決定。手続きは後ほどご案内。",
    U"内定です。おめでとうございます。",
    U"採用となりました。ご入社をお待ちします。",
    U"採用内定。詳細は別途ご連絡します。",
    U"内定決定。おめでとうございます。",
    U"厳正なる選考の結果、内定です。",
  U"採用決定のご案内。続きは追って。",
  U"内定の通知です。おめでとうございます。",
  U"厳正なる選考の結果、貴殿は採用内定といたしました。おめでとうございます。",
  U"選考の結果、貴殿の採用を内定いたしました。詳細は後日ご案内いたします。",
  U"この度の選考により、貴殿を採用内定といたします。続報は追ってご連絡します。",
  U"選考を重ねた結果、貴殿の内定を決定いたしました。おめでとうございます。",
  U"書類審査の結果、貴殿の採用を決定いたしました。入社手続きは追ってご案内します。",
  U"最終選考の結果、貴殿は採用内定となりました。今後の流れは後日ご案内します。",
  U"選考の結果、貴殿の採用を決定しました。必要手続きは別途メールでご案内します。",
  U"厳正なる選考の結果、この度貴殿の採用が内定しました。詳細は後日ご連絡します。",
  U"選考の結果、貴殿を採用といたします。入社に関するご案内は後日お送りします。",
  U"この度は選考の結果、内定といたしました。手続きの詳細は追ってご連絡します。",
  U"書類審査の結果、貴殿は採用といたしました。今後のご案内は後日お知らせします。",
  U"書類審査の結果、採用内定といたしました。おめでとうございます。",
  U"厳正な審査の結果、貴殿の採用内定を決定しました。続報は後日ご案内します。",
  U"書類審査の結果、貴殿の採用を内定しました。入社手順は後日メールでご案内します。",
  U"総合的な選考の結果、貴殿は採用といたします。詳細は後日追ってご連絡します。",
  U"選考の結果、貴殿の採用内定が決定しました。入社に関する詳細は後日ご連絡します。",
  U"この度の選考結果により、貴殿の採用を決定いたしました。おめでとうございます。",
  U"最終選考の結果、貴殿の採用を決定しました。今後の手続きは追ってご案内します。",
  U"慎重な選考の結果、貴殿は採用内定となりました。詳細は後日、別途ご案内します。",
  U"選考の結果、貴殿の採用が内定しました。入社案内は後日、改めてご連絡します。"};

  // お祈りメッセージのリスト
  static inline const Array<String> kRejectionMessages = {
    U"今回は見送りといたします。",
    U"選考の結果、見送りとなりました。",
    U"今回は不採用となりました。",
    U"ご希望に添えず失礼いたします。",
    U"残念ながら見送りの結果です。",
    U"厳正な選考の結果、見送りです。",
    U"採用に至らず、見送りとなりました。",
    U"慎重に検討のうえ、見送りです。",
    U"総合判断の結果、見送りとします。",
    U"今回の募集ではご縁がありませんでした。",
    U"力及ばず、今回は見送りです。",
    U"誠に恐縮ですが、不採用です。",
    U"ご応募に感謝しますが、見送りです。",
    U"選考の結果、採用に至りませんでした。",
    U"今回は貴意に添いかねる結果です。",
    U"たいへん心苦しいのですが見送りです。",
    U"厳正な審査の結果、見送りとなりました。",
    U"大変残念ですが、採用見送りです。",
    U"今回の選考は見送りといたします。",
    U"選考の結果、ご希望に添えません。",
    U"応募内容は拝見しましたが、見送りです。",
    U"申し訳ありませんが、今回は不採用です。",
    U"書類審査の結果、見送りとなりました。",
    U"今回は採用を見送る判断となりました。",
    U"検討の結果、採用を見送りました。",
    U"今回は見送らせていただきます。",
    U"選考の結果、見送らせていただきます。",
    U"今回はご期待に沿えませんでした。",
    U"誠に残念ながら、見送りの結論です。",
    U"またのご応募を心よりお待ちします。",
    U"この度はご応募ありがとうございました。",
    U"多数の応募の中、見送りとなりました。",
    U"総合的に検討の結果、見送りです。",
    U"貴殿の今後のご活躍をお祈りします。",
    U"今回は見送りです。ご理解ください。",
    U"慎重に選考しましたが、見送りです。",
    U"採用選考の結果、今回は見送りです。",
    U"不採用の結果となりました。",
    U"今回は見送りとなりました。ご了承願います。",
  U"恐れ入りますが、今回の採用は見送りです。",
  U"慎重な書類審査の結果、今回は採用見送りとさせていただく判断となりました。",
  U"総合的な書類審査の結果、今回は採用を見送らせていただく結論となりました。",
  U"厳正なる書類選考の結果、誠に恐縮ですが今回は不採用の判断といたしました。",
  U"書類審査を慎重に行いましたが、今回は採用のご希望にお応えできない結果となりました。",
  U"多数の応募を厳正に審査した結果、今回は採用を見送らせていただくこととなりました。",
  U"総合的な観点から書類審査を行い、今回は不採用の判断とさせていただきました。",
  U"書類審査の結果、誠に申し訳ございませんが今回は採用を見送らせていただきます。",
  U"応募内容は拝見いたしましたが、総合的判断の結果、今回は見送りといたしました。",
  U"書類選考の結果、誠に恐縮ですが今回は採用を見送らせていただく判断となりました。",
  U"書類審査の結果、今回はご期待に沿えず採用見送りの判断といたすこととなりました。",
  U"書類審査の結果、誠に恐縮ながら今回は採用の見送りを決定する運びとなりました。",
  U"応募内容は評価いたしましたが、今回は採用を見送る判断とさせていただきました。",
  U"お送りいただいた書類は拝見しましたが、今回は採用に至らない結論となりました。",
  U"厳正に書類審査を実施した結果、今回は採用の見送りとする判断に至りました。",
  U"総合的な観点から慎重に審査し、今回は採用いたしかねる判断といたしました。",
  U"誠に恐縮ではございますが、今回は採用を見送らせていただくこととなりました。",
  U"今回の募集では応募につき、書類審査の結果、採用見送りの判断といたしました。",
  U"書類審査の結果、大変恐縮ですが今回は採用見送りとする結論に至りました。",
  U"慎重に書類審査を進めましたが、今回は採用のご要望にお応えできない結論となりました。"};
};
