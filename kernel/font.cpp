//文字の描画をまとめる。
#include"font.hpp"

//hankaku.oのデータを利用できるようにする。
extern const uint8_t _binary_hankaku_bin_start;
extern const uint8_t _binary_hankaku_bin_end;
extern const uint8_t _binary_hankaku_bin_size;

const uint8_t* GetFont(char c) {
    //文字 c　のアスキーコードの16倍を計算する。その値が、hankaku.o内に含まれる各文字データの先頭アドレスに対応するように作られている。
    auto index = 16 * static_cast<unsigned int>(c);//先頭アドレスを計算する。
    if(index >= reinterpret_cast<uintptr_t>(&_binary_hankaku_bin_size)) {
        return nullptr;
    }
    //ここは推測だがオフセットしているだけだと思われる。
    return &_binary_hankaku_bin_start + index;
}

void WriteAscii(PixelWriter& writer, int x, int y, char c, const PixelColor& color){
    const uint8_t* font = GetFont(c);
    if(font == nullptr){
        return;
    }
    for(int dy=0; dy<16; dy++){
        for(int dx=0; dx<8; dx++){
            if((font[dy]<<dx)&0x80u) {
                writer.Write( x+dx, y+dy, color);
            }
        }
    }
}