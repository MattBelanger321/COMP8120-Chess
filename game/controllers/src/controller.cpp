#include <controller.hpp>
#include <mutex>

namespace chess::controller {
    std::mutex controller::game_mutex;
    chess_game controller::game;

    void controller::select_space( pieces::position_t const & pos )
    {
        std::lock_guard guard( game_mutex );
        selected_space = game.get( pos );
    }

    bool controller::move( game::space const & dst )
    {
        bool ret = false;
        if ( selected_space ) {
            std::lock_guard guard( game_mutex );
            ret = game.move( selected_space.value(), dst );
        }

        selected_space.reset();
        return ret;
    }

}  // namespace chess::controller