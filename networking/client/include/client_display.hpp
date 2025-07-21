#ifndef __CHESS__CLIENT__DISPLAY__
#define __CHESS__CLIENT__DISPLAY__

#include "client_game.hpp"
#include <imgui.h>

#include <functional>
#include <space.hpp>

#include <dear_imgui_chessboard.hpp>

namespace chess::networking {

    struct component_size {
        int width;
        int height;
    };

    struct component_pos {
        int x;
        int y;
    };

    struct component_data {
        component_size size;
        component_pos  pos;
    };

    class client_display {
    public:
        client_display( component_data const board_dims, component_data const status_dims,
                        component_data const control_dims );

        void render();

    private:
        client_game game;

        component_data const board_dims;
        component_data const status_dims;
        component_data const control_dims;

        controller::board_callback get_space;

        std::vector< game::space > possible_moves;

        controller::imgui_chessboard board;

        controller::space_context_t const get( int const i, int const j );
        void                              on_select( game::space const & sp );

        void status_dialog();
        void control_panel();
        void chess_board();
    };
}  // namespace chess::networking

#endif