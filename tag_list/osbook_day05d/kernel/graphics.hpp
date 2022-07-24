#pragma once
//画面への描画方法についてまとめる。
#include "frame_buffer_config.hpp"

//関数及びクラスの宣言をする。

//ピクセルの色変数を定義する。
struct PixelColor {
    uint8_t r, g, b;
};

//スーパークラス : PixelWriter　クラスを宣言。
class PixelWriter {
    public:
     PixelWriter(const FrameBufferConfig& config) : config_{config} {
     }
     virtual ~PixelWriter() = default;
     virtual void Write(int x, int y, const PixelColor& c) = 0;

    protected:
     uint8_t* PixelAt(int x, int y){
        return config_.frame_buffer + 4 * (config_.pixels_per_scan_line * y + x);
     }

    private:
     const FrameBufferConfig& config_;
};

//サブクラスを宣言する。
class RGBResv8BitPerColorPixelWriter : public PixelWriter {
    public:
     using PixelWriter::PixelWriter;
     virtual void Write(int x, int y, const PixelColor& c) override;
};

class BGRResv8BitPerColorPixelWriter : public PixelWriter {
    public:
     using PixelWriter::PixelWriter;
     virtual void Write(int x, int y, const PixelColor& c) override;
};