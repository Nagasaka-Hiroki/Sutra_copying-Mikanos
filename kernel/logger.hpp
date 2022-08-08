/*
*   カーネルロガー
*/

#pragma once

enum LogLevel {
    kError = 3,
    kWarn  = 4,
    kInfo  = 6,
    kDebug = 7,
};

/*  グローバルなログ優先度の閾値を変更する。
*   
*   グローバルなログ優先度の閾値を level に設定する。
*   以降の Log の呼び出しでは、ここで設定した優先度以上のログのみ記録される。
*/
void SetLogLevel(LogLevel level);

/*  ログを指定された優先度で記録する。
*   
*   指定された優先度が閾値以上ならば記録する。
*   優先度が閾値未満ならログは捨てられる。
*   
*   level : ログの優先度。閾値以上の優先度ログのみが記録される。
*   format: 書式文字列。printkと互換。
*/
int Log(LogLevel level, const char* format, ...);
