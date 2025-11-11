---
mode: agent
---

# 参照するファイル
- .vscode/launch.json
- .vscode/settings.json
- .vscode/tasks.json

# タスク内容
launch.jsonで定義されている起動タスクをひとつずつ実行し、ビルドが成功するか確認する。
全ての実行を確認したのちに成功したビルドと失敗したビルドの一覧で表示する。
失敗したビルドがあれば、失敗したビルドのログを簡略化して表示する