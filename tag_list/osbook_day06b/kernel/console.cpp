//コンソールクラスの定義
#include "console.hpp"
#include<cstring>
#include"font.hpp"

//コンストラクタの定義
Console::Console( PixelWriter& writer, 
            const PixelColor& fg_color, 
            const PixelColor& bg_color)
          : writer_{writer},
            fg_color_{fg_color},
            bg_color_{bg_color},
            buffer_{},
            cursor_row_{0},
            cursor_column_{0}{

}
//コンソールに描画するメソッド。
void Console::PutString( const char* s) {
    while(*s){
        if (*s == '\n'){
            //改行文字であれば改行処理をする。
            Newline();
        } else if ( cursor_column_ < kColumns -1) {
            //1文字分描画する。
            WriteAscii(writer_, 8 * cursor_column_, 16 * cursor_row_, *s, fg_color_);
            //描画内容をバッファに保存する。
            buffer_[cursor_row_][cursor_column_]=*s;
            //カーソルを右にずらす。
            cursor_column_++;
        }
        s++;
    }
}
//改行処理
void Console::Newline(){
    //改行した初めはカーソルは左端に来る。
    cursor_column_=0;
    if(cursor_row_<kRows-1){
        //下に描画できるときはカーソルを1つ下にずらす。
        cursor_row_++;
    } else {
        //カーソルが最も下にある時。
        //まずコンソール全体を塗りつぶす
        for(int y=0; y<16*kRows; y++){
            for(int x=0; x<8*kColumns; x++){
                writer_.Write(x,y,bg_color_);
            }
        }
        //一番下の行を空白行にするために文字列配列番号を1つずらす。
        for(int row=0; row<kRows-1; row++){
            //一つずらす。
            memcpy(buffer_[row], buffer_[row+1], kColumns+1);
            //ずらした内容を描画する。
            WriteString(writer_, 0, 16*row, buffer_[row], fg_color_);
        }
        //一番最後の行は前のデータが入っているので0で埋める。
        memset(buffer_[kRows-1], 0, kColumns+1);
    }
}