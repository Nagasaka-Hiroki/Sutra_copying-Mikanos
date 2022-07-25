#pragma once

#include"graphics.hpp"

class Console {
    public:
     //それぞれコンソールの行数列数を表す。
     static const int kRows=25, kColumns=80;
     Console( PixelWriter& writer, const PixelColor& fg_color, const PixelColor& bg_color);
     //コンソールに出力をするメソッド。
     void PutString( const char* s);

    private:
     //改行処理をするメソッド。
     void Newline();

     PixelWriter& writer_;
     const PixelColor fg_color_, bg_color_;
     //表示する文字列を保存するバッファ。kColumns+1の+1はヌル文字('\0')の分を保存するために増やす。
     //再描画の時に使われる。
     char buffer_[kRows][kColumns+1];
     //カーソルの位置を表す。
     int cursor_row_, cursor_column_;
};