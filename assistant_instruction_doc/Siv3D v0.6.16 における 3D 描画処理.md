# Siv3D v0.6.16 における 3D 描画処理

## 基本3Dプリミティブの描画方法とサンプルコード

Siv3D では箱や球などの基本的な3D形状を簡単に描画できます。たとえば Box(cx, cy, cz, size) で直方体を、Sphere(cx, cy, cz, r) で球を表現し、.draw() メソッドで描画します[\[1\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%20%E3%83%9C%E3%83%83%E3%82%AF%E3%82%B9%E3%81%AE%E6%8F%8F%E7%94%BB%20Box%7B,0.8%2C%200.6%2C%200.4%7D.removeSRGBCurve)。以下は一例です：

* **床や図形の描画例:** Plane{64}.draw(uvChecker) で床（平面）を描画し、Box{-8,2,0,4}.draw(color) で位置 (−8,2,0)、一辺長 4 の立方体を描画、Sphere{0,2,0,2}.draw(color) で位置 (0,2,0)、半径 2 の球を描画できます[\[2\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%20%E5%BA%8A%28%3DuvChecker%29%E3%82%92%E6%8F%8F%E7%94%BB%20Plane)。

* **描画フロー:** 3D 描画では、MSRenderTexture などのレンダーテクスチャを作成し、ScopedRenderTarget3D で背景色クリアの上3Dシーンを描きます。その後 Graphics3D::Flush()→renderTexture.resolve()→Shader::LinearToScreen() で2Dシーンに転送して画面に表示します[\[3\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%20renderTexture%E3%82%923D%E6%8F%8F%E7%94%BB%E3%81%AE%E3%83%AC%E3%83%B3%E3%83%80%E3%83%BC%E3%82%BF%E3%83%BC%E3%82%B2%E3%83%83%E3%83%88%E3%81%AB%E3%81%99%E3%82%8B%20const%20ScopedRenderTarget3D%20target)[\[4\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%203D%E3%82%B7%E3%83%BC%E3%83%B3%E3%82%922D%E3%82%B7%E3%83%BC%E3%83%B3%E3%81%AB%E6%8F%8F%E7%94%BB%20)。

これらのプリミティブは色やテクスチャを指定でき、draw(color) や draw(texture, color) で色情報付きで描画できます。

## 3Dカメラの制御（視点移動・回転・マウス操作）

カメラ制御には Camera3D 系のクラスを使います。代表的なものに **DebugCamera3D** があり、ウィンドウサイズ・視野角・初期位置を指定して生成し、update(speed) で操作（キー/マウス）を反映します[\[5\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%203D%E3%82%B7%E3%83%BC%E3%83%B3%E7%94%A8%E3%81%AE%E3%83%87%E3%83%90%E3%83%83%E3%82%B0%E3%82%AB%E3%83%A1%E3%83%A9%20DebugCamera3D%20camera,32)。具体例：

DebugCamera3D camera{renderTexture.size(), 30\_deg, Vec3{10,16,-32}};  
while (System::Update()) {  
    camera.update(2.0); // カメラ移動更新（速度2.0）  
    Graphics3D::SetCameraTransform(camera); // カメラ行列を設定  
    // ここで3D描画を行う...  
}

上記のように camera.update() 後に Graphics3D::SetCameraTransform(camera) を呼ぶことで、以降の3D描画がその視点・向きでレンダリングされます[\[5\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%203D%E3%82%B7%E3%83%BC%E3%83%B3%E7%94%A8%E3%81%AE%E3%83%87%E3%83%90%E3%83%83%E3%82%B0%E3%82%AB%E3%83%A1%E3%83%A9%20DebugCamera3D%20camera,32)。DebugCamera3D はデフォルトで WASD やマウスドラッグで視点移動・回転ができ、ユーザー操作に対応した自由視点カメラが簡単に実現できます。

## ライティングの設定方法と種類

Siv3D の標準シェーダは **環境光（アンビエントライト）** と **平行光（太陽光）** に対応しています。Graphics3D::SetGlobalAmbientColor(ColorF{…}) で環境光の色を、Graphics3D::SetSunColor(ColorF{…}) で太陽光の色を設定します。さらに Graphics3D::SetSunDirection(Vec3{…}.normalized()) で平行光の方向（法線）を設定します[\[6\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=%2F%2F%20%E7%92%B0%E5%A2%83%E5%85%89%E3%82%92%E8%A8%AD%E5%AE%9A%E3%81%99%E3%82%8B%20Graphics3D%3A%3ASetGlobalAmbientColor%28ColorF,75)。例：

Graphics3D::SetGlobalAmbientColor(ColorF{0.75, 0.75, 0.75}); // 環境光  
Graphics3D::SetSunColor       (ColorF{0.5, 0.5, 0.5});       // 太陽光  
Graphics3D::SetSunDirection   (Vec3{0, 1, \-0.3}.normalized()); // 太陽光の方向

上記例では強めの環境光と、地平線上方向 (0,1,-0.3) からの弱めの白色平行光を設定しています[\[6\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=%2F%2F%20%E7%92%B0%E5%A2%83%E5%85%89%E3%82%92%E8%A8%AD%E5%AE%9A%E3%81%99%E3%82%8B%20Graphics3D%3A%3ASetGlobalAmbientColor%28ColorF,75)。Siv3D 標準では **点光源（PointLight）やスポットライト（SpotLight）** は提供されておらず、必要な場合はカスタムシェーダで実装します（フォーラムでも「スポットライトは標準機能に無い」と案内されています）。

## カスタムシェーダ（HLSL）による描画手法

Siv3D ではHLSL/GLSL両対応の PixelShader クラスが用意されており、HLSL シェーダファイルを指定して PixelShader ps \= HLSL{ U"shader.hlsl", U"PS" } のように読み込めます[\[7\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=const%20PixelShader%20ps%20%3D%20HLSL,3%20%7D%20%7D)。例：

const PixelShader ps \= HLSL{ U"example/shader/hlsl/forward\_triplanar.hlsl", U"PS" }  
                    | GLSL{ U"example/shader/glsl/forward\_triplanar.frag", { { U"PSPerFrame", 0 }, … } };

読み込んだシェーダは、3D 描画時に ScopedCustomShader3D スコープオブジェクトで有効化できます。たとえば：

const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };  
const ScopedCustomShader3D shader{ ps }; // カスタムピクセルシェーダを設定  
mesh.draw(texture);  
Graphics3D::Flush();  
renderTexture.resolve();  
Shader::LinearToScreen(renderTexture);

上記のように ScopedCustomShader3D shader{ps} とすると、そのスコープ内の mesh.draw() はカスタムシェーダを用いて描画されます[\[8\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=const%20ScopedRenderTarget3D%20target,ps%20%7D%3B%20meshFront.draw%28woodTexture%29%3B%20meshBack.draw%28woodTexture)。これにより標準ライティング以外の特殊効果（トリプラナ法やライティング計算の変更など）を実現できます。

## モデルファイル（.obj, .gltf など）の読み込みと描画方法

Wavefront .obj フォーマットなどの3Dモデルは **Model** クラスで読み込みます。たとえば自作の box.obj モデルを読み込むには：

const Model boxModel{ U"assets/box.obj" }; // .obj ファイルからモデルを読み込み  
Model::RegisterDiffuseTextures(boxModel, TextureDesc::MippedSRGB); // テクスチャを登録

Model::RegisterDiffuseTextures() は .mtl に依存するテクスチャ (Diffuse マップ) をアセットに登録する関数です[\[9\]](https://massie0414.com/index.php/opensiv3d/11292/#:~:text=%2F%2F%20%E3%83%A2%E3%83%87%E3%83%AB%E3%83%87%E3%83%BC%E3%82%BF%E3%82%92%E3%83%AD%E3%83%BC%E3%83%89%20const%20Model%20boxModel,%2F%2F%20%E8%87%AA%E4%BD%9COBJ)。モデルの描画は .draw() メソッドで行います。例：

const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };  
boxModel.draw(Vec3{0,0,0}); // 座標 (0,0,0) にモデルを描画  
Graphics3D::Flush(); ...

上記のように boxModel.draw(position) で3D空間上にモデルを表示できます[\[10\]](https://massie0414.com/index.php/opensiv3d/11292/#:~:text=%2F%2F%20%5B%E3%83%A2%E3%83%87%E3%83%AB%E3%81%AE%E6%8F%8F%E7%94%BB%5D%20,0%2C%200%2C%200)。Siv3D は OBJ 以外にも glTF 形式（glTF 2.0）の読み込みもサポートしますので、用途に応じて各種フォーマットを利用できます。

## 座標系（右手系/左手系, 原点位置, Y軸方向）

Siv3D の3D座標系は **左手系** です[\[11\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%E5%9F%BA%E6%9C%AC%E7%9A%84%E3%81%AA%E6%A6%82%E5%BF%B5%20%E3%80%9C%E5%BA%A7%E6%A8%99%E7%B3%BB%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6%E3%80%9C)。具体的には、X軸は画面右方向、Y軸は上方向 (垂直上方向) に座標値が増加し、Z軸は手前から奥方向（カメラからスクリーン奥へ）に座標値が増加します[\[11\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%E5%9F%BA%E6%9C%AC%E7%9A%84%E3%81%AA%E6%A6%82%E5%BF%B5%20%E3%80%9C%E5%BA%A7%E6%A8%99%E7%B3%BB%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6%E3%80%9C)。原点は (0,0,0) で、通常シーンの中心や床の交点に置かれることが多いです。頂点座標には Vec3（double型）や Float3（float型）が使われ、描画時には左手系の行列演算で変換されます。なお、Y軸は上方向です（「Y軸上向き」）。

## 数学系クラス（Vec3, Mat4x4 など）の構造

Siv3D では2D/3D向けにさまざまな数学クラスが用意されています。主な3Dベクトル・行列クラスと要素型は次の通りです[\[12\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Disc%20%E5%86%86%E7%9B%A4%20Float3%203%20%E6%AC%A1%E5%85%83%E3%81%AE%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%EF%BC%88%E8%A6%81%E7%B4%A0%E3%81%AF,%EF%BC%89)[\[13\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Vec3%20%E2%98%85%203%20%E6%AC%A1%E5%85%83%E3%81%AE%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%EF%BC%88%E8%A6%81%E7%B4%A0%E3%81%AF%20,%EF%BC%89)[\[14\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Line3D%20%E2%98%85%203D%20%E7%B7%9A%E5%88%86%20Mat4x4,%E2%98%85%204x4%20%E8%A1%8C%E5%88%97)：

* **Float3 / Vec3:** 3 次元ベクトル。Float3 は要素型が float、Vec3 は要素型が double（倍精度）です[\[12\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Disc%20%E5%86%86%E7%9B%A4%20Float3%203%20%E6%AC%A1%E5%85%83%E3%81%AE%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%EF%BC%88%E8%A6%81%E7%B4%A0%E3%81%AF,%EF%BC%89)[\[13\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Vec3%20%E2%98%85%203%20%E6%AC%A1%E5%85%83%E3%81%AE%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%EF%BC%88%E8%A6%81%E7%B4%A0%E3%81%AF%20,%EF%BC%89)。つまり Float3 は 12 バイト（3×4）、Vec3 は 24 バイト（3×8）を占めます。ベクトル要素の順序は (x, y, z) です。

* **Float4 / Vec4:** 4 次元ベクトル（通常は同次座標用）。Float4 は float 4成分（16バイト）、Vec4 は double 4成分（32バイト）です[\[12\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Disc%20%E5%86%86%E7%9B%A4%20Float3%203%20%E6%AC%A1%E5%85%83%E3%81%AE%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%EF%BC%88%E8%A6%81%E7%B4%A0%E3%81%AF,%EF%BC%89)[\[13\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Vec3%20%E2%98%85%203%20%E6%AC%A1%E5%85%83%E3%81%AE%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%EF%BC%88%E8%A6%81%E7%B4%A0%E3%81%AF%20,%EF%BC%89)。要素順序は (x, y, z, w) となります。

* **Mat4x4:** 4×4 行列クラス（要素型は double）。ワールド/ビュー/射影行列などに用います[\[14\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Line3D%20%E2%98%85%203D%20%E7%B7%9A%E5%88%86%20Mat4x4,%E2%98%85%204x4%20%E8%A1%8C%E5%88%97)。行列のメモリ上の格納順序（行優先か列優先か）は公式ドキュメントに明記がありませんが、演算時には DirectX 互換の左手系行列演算が内部で行われます。

これらクラスは演算子オーバーロードも提供しており、ベクトル同士や行列との乗算、スカラー乗算などが可能です。

## 用意されている数学関数（内積・外積・正規化・補間・射影など）

Vec3/Vec4 にはベクトル演算用のメソッドが豊富に実装されています。たとえば **内積** や **外積**、**正規化** は Vec3::Dot(a, b), Vec3::Cross(a, b), a.normalized() のように利用できます[\[15\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=%2F%2F%20%E5%A4%AA%E9%99%BD%E3%81%AE%E6%96%B9%E5%90%91%E3%82%92%E8%A8%AD%E5%AE%9A%E3%81%99%E3%82%8B%20Graphics3D%3A%3ASetSunDirection%28Vec3,normalized)。上記の例では Vec3{0,1,-0.3}.normalized() で正規化ベクトルを取得しています[\[15\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=%2F%2F%20%E5%A4%AA%E9%99%BD%E3%81%AE%E6%96%B9%E5%90%91%E3%82%92%E8%A8%AD%E5%AE%9A%E3%81%99%E3%82%8B%20Graphics3D%3A%3ASetSunDirection%28Vec3,normalized)。また線形補間（**lerp**）や投影（**project**）なども Vec3 や Mat4x4 にサポートがあります。

行列関連では、ワールド・ビュー・射影行列の生成メソッドが用意されています。たとえば回転行列・平行移動行列・拡大縮小行列は静的メソッドから生成できます[\[16\]](https://massie0414.com/index.php/opensiv3d/11306/#:~:text=%2F%2F%20%E7%A7%BB%E5%8B%95%E8%A1%8C%E5%88%97%E3%82%92%E4%BD%9C%E3%82%8B%20Mat4x4%20translate%20%3D,0%2C%200%2C%200)：

Mat4x4 translate \= Mat4x4::Translate(x, y, z); // 平行移動行列    
Mat4x4 rotateX    \= Mat4x4::RotateX(radians);   // X回転行列    
Mat4x4 rotateY    \= Mat4x4::RotateY(radians);   // Y回転行列    
Mat4x4 rotateZ    \= Mat4x4::RotateZ(radians);   // Z回転行列    
Mat4x4 scale      \= Mat4x4::Scale(s);          // 拡大縮小行列    
Mat4x4 transform  \= translate \* rotateX \* rotateY \* rotateZ \* scale; // 合成行列  

実際の描画時に上記のような行列を計算し、mesh.draw(transform) のように渡すことでモデルの変換が可能です[\[16\]](https://massie0414.com/index.php/opensiv3d/11306/#:~:text=%2F%2F%20%E7%A7%BB%E5%8B%95%E8%A1%8C%E5%88%97%E3%82%92%E4%BD%9C%E3%82%8B%20Mat4x4%20translate%20%3D,0%2C%200%2C%200)。

これらの数学クラスと関数により、座標変換・投影・ライティング計算など 3D グラフィックスに必要な数学演算を容易に扱うことができます。

**参考資料:** Siv3D 公式リファレンスやサンプルコード[\[11\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%E5%9F%BA%E6%9C%AC%E7%9A%84%E3%81%AA%E6%A6%82%E5%BF%B5%20%E3%80%9C%E5%BA%A7%E6%A8%99%E7%B3%BB%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6%E3%80%9C)[\[6\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=%2F%2F%20%E7%92%B0%E5%A2%83%E5%85%89%E3%82%92%E8%A8%AD%E5%AE%9A%E3%81%99%E3%82%8B%20Graphics3D%3A%3ASetGlobalAmbientColor%28ColorF,75)[\[17\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=const%20ScopedRenderTarget3D%20target,ps%20%7D%3B%20meshFront.draw%28woodTexture)[\[9\]](https://massie0414.com/index.php/opensiv3d/11292/#:~:text=%2F%2F%20%E3%83%A2%E3%83%87%E3%83%AB%E3%83%87%E3%83%BC%E3%82%BF%E3%82%92%E3%83%AD%E3%83%BC%E3%83%89%20const%20Model%20boxModel,%2F%2F%20%E8%87%AA%E4%BD%9COBJ)[\[7\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=const%20PixelShader%20ps%20%3D%20HLSL,3%20%7D%20%7D)[\[12\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Disc%20%E5%86%86%E7%9B%A4%20Float3%203%20%E6%AC%A1%E5%85%83%E3%81%AE%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%EF%BC%88%E8%A6%81%E7%B4%A0%E3%81%AF,%EF%BC%89)、および GitHub 実装（OpenSiv3D）など。各クラスやメソッドの詳細は公式ドキュメントを参照してください。

---

[\[1\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%20%E3%83%9C%E3%83%83%E3%82%AF%E3%82%B9%E3%81%AE%E6%8F%8F%E7%94%BB%20Box%7B,0.8%2C%200.6%2C%200.4%7D.removeSRGBCurve) [\[2\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%20%E5%BA%8A%28%3DuvChecker%29%E3%82%92%E6%8F%8F%E7%94%BB%20Plane) [\[3\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%20renderTexture%E3%82%923D%E6%8F%8F%E7%94%BB%E3%81%AE%E3%83%AC%E3%83%B3%E3%83%80%E3%83%BC%E3%82%BF%E3%83%BC%E3%82%B2%E3%83%83%E3%83%88%E3%81%AB%E3%81%99%E3%82%8B%20const%20ScopedRenderTarget3D%20target) [\[4\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%203D%E3%82%B7%E3%83%BC%E3%83%B3%E3%82%922D%E3%82%B7%E3%83%BC%E3%83%B3%E3%81%AB%E6%8F%8F%E7%94%BB%20) [\[5\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%2F%2F%203D%E3%82%B7%E3%83%BC%E3%83%B3%E7%94%A8%E3%81%AE%E3%83%87%E3%83%90%E3%83%83%E3%82%B0%E3%82%AB%E3%83%A1%E3%83%A9%20DebugCamera3D%20camera,32) [\[11\]](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html#:~:text=%E5%9F%BA%E6%9C%AC%E7%9A%84%E3%81%AA%E6%A6%82%E5%BF%B5%20%E3%80%9C%E5%BA%A7%E6%A8%99%E7%B3%BB%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6%E3%80%9C) OpenSiv3Dに追加された3D描画機能を試してみた | 為せばnull

[https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html](https://blog.yotio.jp/2021/10/06/OpenSiv3D%E3%81%AB%E8%BF%BD%E5%8A%A0%E3%81%95%E3%82%8C%E3%81%9F3D%E6%8F%8F%E7%94%BB%E6%A9%9F%E8%83%BD%E3%82%92%E8%A9%A6%E3%81%97%E3%81%A6%E3%81%BF%E3%81%9F.html)

[\[6\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=%2F%2F%20%E7%92%B0%E5%A2%83%E5%85%89%E3%82%92%E8%A8%AD%E5%AE%9A%E3%81%99%E3%82%8B%20Graphics3D%3A%3ASetGlobalAmbientColor%28ColorF,75) [\[7\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=const%20PixelShader%20ps%20%3D%20HLSL,3%20%7D%20%7D) [\[8\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=const%20ScopedRenderTarget3D%20target,ps%20%7D%3B%20meshFront.draw%28woodTexture%29%3B%20meshBack.draw%28woodTexture) [\[15\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=%2F%2F%20%E5%A4%AA%E9%99%BD%E3%81%AE%E6%96%B9%E5%90%91%E3%82%92%E8%A8%AD%E5%AE%9A%E3%81%99%E3%82%8B%20Graphics3D%3A%3ASetSunDirection%28Vec3,normalized) [\[17\]](https://siv3d.github.io/ja-jp/samples/3d/#:~:text=const%20ScopedRenderTarget3D%20target,ps%20%7D%3B%20meshFront.draw%28woodTexture) 3D 描画のサンプル \- Siv3D

[https://siv3d.github.io/ja-jp/samples/3d/](https://siv3d.github.io/ja-jp/samples/3d/)

[\[9\]](https://massie0414.com/index.php/opensiv3d/11292/#:~:text=%2F%2F%20%E3%83%A2%E3%83%87%E3%83%AB%E3%83%87%E3%83%BC%E3%82%BF%E3%82%92%E3%83%AD%E3%83%BC%E3%83%89%20const%20Model%20boxModel,%2F%2F%20%E8%87%AA%E4%BD%9COBJ) [\[10\]](https://massie0414.com/index.php/opensiv3d/11292/#:~:text=%2F%2F%20%5B%E3%83%A2%E3%83%87%E3%83%AB%E3%81%AE%E6%8F%8F%E7%94%BB%5D%20,0%2C%200%2C%200) OpenSiv3D：自作したOBJファイルを描画してみる \- massie's programming

[https://massie0414.com/index.php/opensiv3d/11292/](https://massie0414.com/index.php/opensiv3d/11292/)

[\[12\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Disc%20%E5%86%86%E7%9B%A4%20Float3%203%20%E6%AC%A1%E5%85%83%E3%81%AE%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%EF%BC%88%E8%A6%81%E7%B4%A0%E3%81%AF,%EF%BC%89) [\[13\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Vec3%20%E2%98%85%203%20%E6%AC%A1%E5%85%83%E3%81%AE%E3%83%99%E3%82%AF%E3%83%88%E3%83%AB%EF%BC%88%E8%A6%81%E7%B4%A0%E3%81%AF%20,%EF%BC%89) [\[14\]](https://siv3d.github.io/ja-jp/api/classes/#:~:text=Line3D%20%E2%98%85%203D%20%E7%B7%9A%E5%88%86%20Mat4x4,%E2%98%85%204x4%20%E8%A1%8C%E5%88%97) クラス一覧 \- Siv3D

[https://siv3d.github.io/ja-jp/api/classes/](https://siv3d.github.io/ja-jp/api/classes/)

[\[16\]](https://massie0414.com/index.php/opensiv3d/11306/#:~:text=%2F%2F%20%E7%A7%BB%E5%8B%95%E8%A1%8C%E5%88%97%E3%82%92%E4%BD%9C%E3%82%8B%20Mat4x4%20translate%20%3D,0%2C%200%2C%200) OpenSiv3D：Blenderを使って背面法によるアウトラインを表示している \- massie's programming

[https://massie0414.com/index.php/opensiv3d/11306/](https://massie0414.com/index.php/opensiv3d/11306/)