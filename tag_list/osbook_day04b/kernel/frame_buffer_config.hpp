#pragma once

#include<stdint.h>
//2つのピクセルフォーマットを選択。
enum PixelFormat {
    kPixelRGBResv8BitPerColor,
    kPixelBGRResv8BitRepColor,
};
//描画に関する情報を構造体にまとめる。
struct FrameBufferConfig {
    uint8_t* frame_buffer;
    uint32_t pixel_per_scan_line;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    enum PixelFormat pixel_format;
};