#pragma once

#include<cstdint>
#include"graphics.hpp"

void WriteAscii(PixelWriter& writer, int x, int y, char c, const PixelColor& color);
//文字列で描画する
void WriteString(PixelWriter& writer, int x, int y, const char* s,const PixelColor& color);