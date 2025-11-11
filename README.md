# Siv3DGameJam202510_public

このプロジェクトは、Siv3DGameJam2025で開発した「2031/06/20-23」のリポジトリから再配布不可のアセットを省いた公開リポジトリです。

## ビルド環境構築方法

1. Siv3Dのv0.6.16をインストールします。もしくは、Siv3Dの最新版をインストールし、Siv3DFrameWorkNoCuda.vcxproj/Siv3DFrameWork.vcxprojのSiv3Dのバージョンに対応した部分を最新のバージョン番号に書き換えます
2. 「Qwen3-4B-Instruct-2507.gguf」を./App/LlmModel直下に移動します
   - 「Qwen3-4B-Instruct-2507.gguf」はHugging FaceのQwen3のページからDLしてきたSafetensors形式のファイルをllama.cppを用いてggufに変換するか、Hugging Faceのunslothのページにある変換済みのggufを使ってください
3. CUDA使用可能な環境であれば、Siv3DFrameWork.vcxprojをビルドすることでCUDA対応の実行ファイルが./App以下に生成されます。CUDA使用不可の環境であれば、Siv3DFrameWorkNoCuda.vcxprojをビルドしてください
   - ReleaseビルドではNoCudaでも十分な速度が出る想定です

## ライセンス表記

ビルド実行中にF1キーを押すことで表示が行われます

## 使用素材

    Springin’ Sound Stock
    https://www.springin.org/sound-stock/
    タイピング メカニカル単1, タイピング メカニカル単2, タイピング メカニカル単3, タイピング パンタグラフ単1, wrench02
    Classix
    https://classix.sitefactory.info/index.html
    Ｇ線上のアリア, 花のワルツ, ハンガリー舞曲, 月の光
    ICOOON MONO
    https://icooon-mono.com/
    目アイコン6
    ChatGPT Sora
    https://chatgpt.com/
    gom_tex, checker_board_tex, metal_board_tex, amusement_background
    Gemini Flash Image
    https://gemini.google.com/
    metal_roung_tex.png