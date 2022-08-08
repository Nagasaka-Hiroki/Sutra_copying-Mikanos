/*
    カーネル
*/

#include<cstdint>
#include<cstddef>
#include<cstdio>

#include"frame_buffer_config.hpp"
#include"graphics.hpp"
#include"font.hpp"
#include"console.hpp"
#include"pci.hpp"

const PixelColor kDesktopBGColor{  45, 118, 237};
const PixelColor kDesktopFGColor{ 255, 255, 255};

//マウスカーソル
const int kMouseCursorWidth=15;
const int kMouseCursorHeight=24;
//kMouseCursorWidthは文字数を表す。
//kMouseCursorHeightは文字変数の数。
const char mouse_cursor_shape[kMouseCursorHeight][kMouseCursorWidth+1]={
/*行数と列数をコメントで表示する。*/
/* 行数  \    列数    123456789abcdef    */
/*   1   */         "@              ",
/*   2   */         "@@             ",
/*   3   */         "@.@            ",
/*   4   */         "@..@           ",
/*   5   */         "@...@          ",
/*   6   */         "@....@         ",
/*   7   */         "@.....@        ",
/*   8   */         "@......@       ",
/*   9   */         "@.......@      ",
/*  10   */         "@........@     ",
/*  11   */         "@.........@    ",
/*  12   */         "@..........@   ",
/*  13   */         "@...........@  ",
/*  14   */         "@............@ ",
/*  15   */         "@......@@@@@@@@",
/*  16   */         "@......@       ",
/*  17   */         "@....@@.@      ",
/*  18   */         "@...@ @.@      ",
/*  19   */         "@..@   @.@     ",
/*  20   */         "@.@    @.@     ",
/*  21   */         "@@      @.@    ",
/*  22   */         "@       @.@    ",
/*  23   */         "         @.@   ",
/*  24   */         "         @@@   ",
};

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
    
    //画面サイズを取得
    const int kFrameWidth=frame_buffer_config.horizontal_resolution;
    const int kFrameeHeight=frame_buffer_config.vertical_resolution;
    //OSぽい見た目にしてみる。
    FillRectangle(*pixel_writer,
                    {0,0},
                    {kFrameWidth, kFrameeHeight-50},
                    kDesktopBGColor);
    FillRectangle(*pixel_writer,
                    {0, kFrameeHeight-50},
                    {kFrameWidth,50},
                    {1,8,17});
    FillRectangle(*pixel_writer,
                    {0,kFrameeHeight-50},
                    {kFrameWidth/5, 50},
                    {80,80,80});
    FillRectangle(*pixel_writer,
                    {10, kFrameeHeight-40},
                    {30,30},
                    {160,160,160});
    
    //コンソールをグローバル変数として使用する
    //配置newでメモリを確保して使用する。
    console=new(console_buf) Console{*pixel_writer, kDesktopFGColor, kDesktopBGColor};
    printk("Welcom to Mikanos!\n");
    //マウスカーソルの描画
    for(int dy=0; dy<kMouseCursorHeight; dy++){
        for(int dx=0; dx<kMouseCursorWidth; dx++){
            if(mouse_cursor_shape[dy][dx]=='@'){
                pixel_writer->Write(200+dx, 100+dy, { 0, 0, 0});
            } else if (mouse_cursor_shape[dy][dx]=='.'){
                pixel_writer->Write(200+dx, 100+dy, { 255, 255, 255});
            }
        }
    }

    //PCIデバイスの一覧を取得して表示する。
    auto err = pci::ScanAllBus();
    printk("ScanAllBus: %s\n", err.Name());

    for(int i=0; i<pci::num_device; i++){
        const auto& dev = pci::devices[i];
        auto vendor_id = pci::ReadVendorId(dev.bus, dev.decive, dev.function);
        auto class_code = pci::ReadClassCode(dev.bus, dev.decive, dev.function);
        printk("%d.%d.%d: vend %04x, class %08x, head %02x\n",
        dev.bus, dev.decive, dev.function,
        vendor_id, class_code, dev.header_type);
    }

    while(1) __asm__("hlt");
}