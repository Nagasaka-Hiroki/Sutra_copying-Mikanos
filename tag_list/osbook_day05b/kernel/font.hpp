#pragma once

#include<cstdint>
#include"graphics.hpp"
//文字を描画する関数を定義する。
void WriteAscii(PixelWriter& writer, int x, int y, char c, const PixelColor& color);