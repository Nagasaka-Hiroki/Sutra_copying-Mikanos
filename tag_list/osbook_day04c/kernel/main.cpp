/*
    カーネル
*/

#include<cstdint>
#include<cstddef>
#include"frame_buffer_config.hpp"

struct PixelColor {
    uint8_t r,g,b;
};
//PixelWriter class
class PixelWriter {
    public:
     PixelWriter(const FrameBufferConfig& config) : config_{config} {
     }
     virtual ~PixelWriter() = default;
     virtual void Write(int x, int y, const PixelColor& c) = 0;
    
    protected:
     uint8_t* PixelAt(int x, int y) {
        return config_.frame_buffer + 4 * (config_.pixels_per_scan_line * y + x);
     }
    
    private:
     const FrameBufferConfig& config_;
};
//PixelWriter class を継承しピクセルフォーマットが異なるクラスをそれぞれ定義する。
//ピクセルフォーマットがPixelRGBResv8BitPerColorの場合。
class RGBResv8BitPerColorWriter : public PixelWriter {
    public:
     using PixelWriter::PixelWriter;//コンストラクタの代わり。
     
     virtual void Write(int x, int y, const PixelColor& c) override {
        auto p = PixelAt(x,y);
        p[0] = c.r;
        p[1] = c.g;
        p[2] = c.b;
     }
};
//ピクセルフォーマットがPixelBGRResv8BitRepColorの場合。
class BGRResv8BitPerColorWriter : public PixelWriter {
    public:
     using PixelWriter::PixelWriter;

     virtual void Write(int x, int y, const PixelColor& c) override {
        auto p = PixelAt(x,y);
        p[0] = c.b;
        p[1] = c.g;
        p[2] = c.r;
     }
};

//演算子の定義 new
void* operator new(size_t size, void* buf){
    return buf;
}
//演算子の定義 delete
void operator delete(void* obj) noexcept{
}

//グローバル変数を定義。
char pixel_writer_buf[sizeof(RGBResv8BitPerColorWriter)];
PixelWriter* pixel_writer;//スーパークラスで変数を宣言する。

extern "C" void KernelMain(const FrameBufferConfig& frame_buffer_config) {
    //サポートしているピクセルフォーマットに従ってインスタンスを生成する。
    switch (frame_buffer_config.pixel_format) {
        case kPixelRGBResv8BitPerColor:
            pixel_writer = new(pixel_writer_buf) RGBResv8BitPerColorWriter{frame_buffer_config};
            break;
        case kPixelBGRResv8BitPerColor:
            pixel_writer = new(pixel_writer_buf) BGRResv8BitPerColorWriter{frame_buffer_config}; 
            break;
    }
    //背景を白く塗る。
    for(int x=0; x<frame_buffer_config.horizontal_resolution; x++){
        for(int y=0; y<frame_buffer_config.vertical_resolution; y++){
            pixel_writer->Write(x, y, {255,255,255});
        }
    }
    //任意の領域を緑に塗る
    for(int x=0; x<200; x++){
        for(int y=0; y<100; y++){
            pixel_writer->Write(x , y, {0, 255, 0});
        }
    }

    while(1) __asm__("hlt");
}