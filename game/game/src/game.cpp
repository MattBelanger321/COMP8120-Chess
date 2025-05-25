#include <game.hpp>

namespace chess {

    void chess_game::move( game::space const & src, game::space const & dst )
    {
        game_board.move( src.position(), dst.position() );
    }
}  // namespace chess