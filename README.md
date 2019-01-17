# ProjectWork

プロジェクトワークのライントレースプログラムです。  
このコードを使用する際は、プログラムのコードに以下のような記述をしてください。

---

Copyright (c) 2019 Mokuichi147  
Released under the MIT license  
https://opensource.org/licenses/mit-license.php

---

## 環境
- モーター 2個
- 光センサ 4個
- サーボモータ無

## 機体について

PORTBbits.RB7 -> 右のモーター前進  
PORTBbits.RB4 -> 左のモーター前進  
PORTBbits.RB6 -> 右のモーター後進  
PORTBbits.RB5 -> 左のモーター後進

## 内容
- Pro.c : いろんなことを試すのに使っていた。内容はStandard7.cに近い。
- Standard7.c : スタンダードコース用のプログラム。早めになってる。
- Technical7.c : テクニカルコース用のプログラム。ゆっくり確実に進む。
- test.c : 対話型でセンサーの値に対するコースがわかる。utf-8で実行できないと文字化けする。
- Overnight.c : 動作未保証の一晩で作成した、Standard7.cとは進み方が違うプログラム。
