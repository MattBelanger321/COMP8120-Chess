#ifndef __CHESS__CONTROLLER__DISPLAY_CONTROLLER__
#define __CHESS__CONTROLLER__DISPLAY_CONTROLLER__

#include <controller.hpp>
#include <imgui.h>

#include <functional>
#include <space.hpp>

#include <dear_imgui_chessboard.hpp>

namespace chess::controller {

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

    class display : public controller {
    public:
        display( component_data const board_dims, component_data const status_dims, component_data const control_dims );
        display( component_data const board_dims, component_data const status_dims, component_data const control_dims,
                 std::string const & board_state );

        void render();

    private:
        component_data const board_dims;
        component_data const status_dims;
        component_data const control_dims;

        board_callback get_space;

        std::vector< game::space > possible_moves;

        imgui_chessboard board;

        space_context_t const get( int const i, int const j );
        void                  on_select( game::space const & sp );

        void status_dialog();
        void control_panel();
        void chess_board();
    };
}  // namespace chess::controller

#endif