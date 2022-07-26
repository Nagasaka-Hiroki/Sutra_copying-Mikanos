/*
    カーネル
*/

#include<cstdint>
#include<cstddef>
#include<cstdio>

//分割したファイルのヘッダファイルをインクルードする。
#include"frame_buffer_config.hpp"
#include"graphics.hpp"
#include"font.hpp"
#include"console.hpp"

void* operator new(size_t size, void* buf){
    return buf;
}

void operator delete(void* obj) noexcept{
}

char pixel_writer_buf[sizeof(RGBResv8BitPerColorPixelWriter)];
PixelWriter* pixel_writer;

//コンソールクラスをグローバル変数として設定する。
char console_buf[sizeof(Console)];
Console* console;
//カーネルからのメッセージを出力する。
int printk(const char* format, ...){
    va_list ap;
    int result;
    char s[1024];//描画できる文字数に限りがある。
    //フォーマットを1つの変数として扱う。
    //可変長引数の受け取りを開始する。
    va_start(ap, format);
    //フォーマットに従った文字列を生成する
    result=vsprintf(s, format, ap);
    //可変長引数の受け取りを終了する。
    va_end(ap);
    //コンソールに文字列を描画する。
    console->PutString(s);
    
    return result;
}

extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
    switch (frame_buffer_config.pixel_format) {
        case kPixelRGBResv8BitPerColor:
            pixel_writer = new(pixel_writer_buf) RGBResv8BitPerColorPixelWriter{frame_buffer_config};
            break;
        case kPixelBGRResv8BitPerColor:
            pixel_writer = new(pixel_writer_buf) BGRResv8BitPerColorPixelWriter{frame_buffer_config}; 
            break;
    }

    for(int x=0; x<frame_buffer_config.horizontal_resolution; x++){
        for(int y=0; y<frame_buffer_config.vertical_resolution; y++){
            pixel_writer->Write(x, y, {255,255,255});
        }
    }

    //コンソールをグローバル変数として使用する
    //配置newでメモリを確保して使用する。
    console=new(console_buf) Console{*pixel_writer, { 0, 0, 0}, { 255, 255, 255}};
    
    //printkを使ってみる
    for(int i=0; i<27; i++){
        printk("printk: %d\n", i);
    }

    while(1) __asm__("hlt");
}