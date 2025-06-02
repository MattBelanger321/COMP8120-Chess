#ifndef __CHESS__CONTROLLER__IMGUI_CHESSBOARD__
#define __CHESS__CONTROLLER__IMGUI_CHESSBOARD__

#include <GL/gl.h>
#include <filesystem>
#include <functional>
#include <imgui.h>
#include <space.hpp>
#include <unordered_map>

namespace chess::controller {

    struct gl_img_t {
        int    width;
        int    height;
        GLuint img;
    };

    struct space_context_t {
        game::space const & sp;
        bool                possible;  // set if a piece is selected AND that piece can move to this space
    };

    enum class icon_t {
        white_king,
        white_queen,
        white_rook,
        white_bishop,
        white_knight,
        white_pawn,

        black_king,
        black_queen,
        black_rook,
        black_bishop,
        black_knight,
        black_pawn
    };

    using board_callback     = std::function< space_context_t const( int const i, int const j ) >;
    using selection_callback = std::function< void( game::space const & ) >;

    class imgui_chessboard {
    public:
        imgui_chessboard( unsigned int const width, unsigned int const height, board_callback const game_board,
                          selection_callback const select );

        void render();

    private:
        unsigned int width;
        unsigned int height;

        board_callback const     game_board;
        selection_callback const select;

        std::unordered_map< icon_t, gl_img_t > icons;

        void draw_board();
        void draw_square( space_context_t const & sp );

        icon_t detect_icon( const std::filesystem::path & path ) const;

        void init();
    };
}  // namespace chess::controller

#endif