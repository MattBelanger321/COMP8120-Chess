#ifndef __CHESS__CONTROLLER__IMGUI_CHESSBOARD__
#define __CHESS__CONTROLLER__IMGUI_CHESSBOARD__

#include <GL/gl.h>
#include <filesystem>
#include <functional>
#include <imgui.h>
#include <space.hpp>
#include <unordered_map>

namespace chess::controller {
    using board_callback = std::function< game::space const &( int const i, int const j ) >;

    struct gl_img_t {
        int    width;
        int    height;
        GLuint img;
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

    class imgui_chessboard {
    public:
        imgui_chessboard( unsigned int const width, unsigned int const height, board_callback const game_board );

        void render();

    private:
        unsigned int         width;
        unsigned int         height;
        board_callback const game_board;

        std::unordered_map< icon_t, gl_img_t > icons;

        void draw_board();
        void draw_square( game::space const & sp );

        icon_t detect_icon( const std::filesystem::path & path ) const;

        void init();
    };
}  // namespace chess::controller

#endif