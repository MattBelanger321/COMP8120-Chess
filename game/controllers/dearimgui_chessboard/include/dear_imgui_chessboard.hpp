#ifndef __CHESS__CONTROLLER__IMGUI_CHESSBOARD__
#define __CHESS__CONTROLLER__IMGUI_CHESSBOARD__

#include <functional>
#include <imgui.h>
#include <space.hpp>

namespace chess::controller {
    using board_callback = std::function< game::space const &( int const i, int const j ) >;

    class imgui_chessboard {
    public:
        imgui_chessboard( unsigned int const width, unsigned int const height, board_callback const game_board );

        void render();

    private:
        unsigned int         width;
        unsigned int         height;
        board_callback const game_board;

        void draw_board();
        void draw_square( game::space const & sp );
    };
}  // namespace chess::controller

#endif