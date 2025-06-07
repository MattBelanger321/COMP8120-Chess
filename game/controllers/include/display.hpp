#ifndef __CHESS__CONTROLLER__DISPLAY_CONTROLLER__
#define __CHESS__CONTROLLER__DISPLAY_CONTROLLER__

#include <controller.hpp>
#include <imgui.h>

#include <functional>
#include <space.hpp>

#include <dear_imgui_chessboard.hpp>

namespace chess::controller {

    class display : public controller {
    public:
        display( unsigned int width, unsigned int height );

        void render();

    private:
        unsigned int   width;
        unsigned int   height;
        board_callback get_space;

        std::vector< game::space > possible_moves;

        imgui_chessboard board;

        space_context_t const get( int const i, int const j );
        void                  on_select( game::space const & sp );

        void status_dialog();
        void chess_board();
    };
}  // namespace chess::controller

#endif