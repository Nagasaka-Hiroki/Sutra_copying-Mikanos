#include"mouse.hpp"
#include"graphics.hpp"

namespace {
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

    void DrawMouseCursor(PixelWriter* pixel_writer, Vector2D<int> position) {
        for (int dy=0; dy<kMouseCursorHeight; dy++){
            for(int dx=0; dx<kMouseCursorWidth; dx++){
                if(mouse_cursor_shape[dy][dx]=='@'){
                    pixel_writer->Write(position.x+dx, position.y+dy, { 0, 0, 0});
                } else if(mouse_cursor_shape[dy][dx]=='.'){
                    pixel_writer->Write(position.x+dx, position.y+dy, { 255, 255, 255});
                }
            }
        }
    }

    void EraseMouseCursor(PixelWriter* pixel_writer, Vector2D<int> position, PixelColor erase_color){
        for(int dy=0; dy<kMouseCursorHeight; dy++){
            for(int dx=0; dx<kMouseCursorWidth; dx++){
                if(mouse_cursor_shape[dy][dx]!=' '){
                    pixel_writer->Write(position.x+dx, position.y+dy, erase_color);
                }
            }
        }
    }
}

MouseCursor::MouseCursor(PixelWriter* writer, PixelColor erase_color, Vector2D<int> initial_position)
        : pixel_writer_{writer},
          erase_color_{erase_color},
          position_{initial_position} {
    DrawMouseCursor(pixel_writer_, position_);
}

void MouseCursor::MoveRelative(Vector2D<int> displacement) {
    EraseMouseCursor(pixel_writer_, position_, erase_color_);
    position_ += displacement;
    DrawMouseCursor(pixel_writer_, position_);
}