#include "piece.hpp"
#include <controller.hpp>
#include <iostream>
#include <mutex>

namespace chess::controller {
    std::mutex controller::game_mutex;
    chess_game controller::game;

    void controller::select_space( pieces::position_t const & pos )
    {
        std::lock_guard guard( game_mutex );
        selected_space = game.get( pos );
    }

    pieces::move_status controller::move( game::space const & dst )
    {
        auto ret = pieces::move_status::valid;
        if ( selected_space ) {
            std::lock_guard guard( game_mutex );
            ret = game.move( selected_space.value(), dst );
        }
        else {
            ret = pieces::move_status::no_space_to_move_from;
        }

        selected_space.reset();
        return ret;
    }

}  // namespace chess::controller