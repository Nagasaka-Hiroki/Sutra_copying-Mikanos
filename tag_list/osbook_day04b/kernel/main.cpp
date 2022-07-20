/*
    カーネル
*/

#include<cstdint>
#include<cstddef>
#include"frame_buffer_config.hpp"

//現在想定しているピクセルフォーマットの場合RGB各色がそれぞれ8bitづつ持てるのでuint8_tを使う
struct PixelColor {
    uint8_t r,g,b;
};
//プロトタイプ宣言。みかん本はKernelMainから説明されるので、説明の順番通りに書く場合プロトタイプ宣言しないとエラーが出る。
int WritePixel(const FrameBufferConfig& config, int x, int y, const PixelColor& c);

//みかん本で説明されている順番に関数を書いていく。
//KernelMain ブートローダ側で引数を改造したのでそれに合わせる。c++の参照を使う。
extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
    //画面を白く塗るところを参照で書き換える。
    for(int x=0; x<frame_buffer_config.horizontal_resolution; x++){
        for( int y=0; y<frame_buffer_config.vertical_resolution; y++){
            WritePixel(frame_buffer_config, x, y, {255,255,255});
        }
    }
    //四角の領域を{0,255,0}で塗りつぶす。おそらく、環境によらず緑になるはず。
    for(int x=0; x<200; x++){
        for(int y=0; y<100; y++){
            WritePixel(frame_buffer_config, 100+x , 100+y, {0, 255, 0});
        }
    }

    while(1) __asm__("hlt");
}
//範囲を指定してピクセルを書く。
int WritePixel(const FrameBufferConfig& config, int x, int y, const PixelColor& c) {
    const int pixel_position = config.pixel_per_scan_line * y + x;
    if(config.pixel_format == kPixelRGBResv8BitPerColor){
        uint8_t* p = &config.frame_buffer[4 * pixel_position];
        p[0] = c.r;
        p[1] = c.g;
        p[2] = c.b;
    } else if(config.pixel_format == kPixelBGRResv8BitRepColor){
        uint8_t* p = &config.frame_buffer[4 * pixel_position];
        p[0] = c.b;
        p[1] = c.g;
        p[2] = c.r;
    } else {
        return -1;
    }
    return 0;
}