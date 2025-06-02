#ifndef __CHESS__CONTROLLER__IMGUI_CHESSBOARD__
#define __CHESS__CONTROLLER__IMGUI_CHESSBOARD__

#include <imgui.h>

namespace chess::controller {
    class imgui_chessboard {
    public:
        imgui_chessboard( unsigned int width, unsigned int height );

        void render();

    private:
        unsigned int width;
        unsigned int height;

        void draw_board();
        void draw_square( int const i, int const j );
    };
}  // namespace chess::controller

#endif