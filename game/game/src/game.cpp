#include "board.hpp"
#include <game.hpp>
#include <string>

namespace chess {

    void chess_game::move( game::space const & src, game::space const & dst )
    {
        game_board.move( src.position(), dst.position() );
    }

    game::space const & chess_game::get( pieces::position_t const & pos ) const { return game_board.get( pos ); }

    std::string chess_game::to_string() const { return game_board.to_string(); }

    std::vector< game::space > chess_game::possible_moves( game::space const & src ) const
    {
        return game_board.possible_moves( src );
    }

}  // namespace chess