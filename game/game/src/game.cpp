#include "board.hpp"
#include <algorithm>
#include <game.hpp>
#include <string>

namespace chess {

    chess_game::chess_game() { start(); }

    void chess_game::start()
    {
        state = game_state::white_move;
        game_board.reset();
    }

    bool chess_game::move( game::space const & src, game::space const & dst )
    {
        auto const & pos = src.possible_moves();

        if ( std::find( pos.begin(), pos.end(), dst ) == pos.end() ) {
            return false;
        }

        game_board.move( src.position(), dst.position() );
        return true;
    }

    game::space const & chess_game::get( pieces::position_t const & pos ) const { return game_board.get( pos ); }

    std::string chess_game::to_string() const { return game_board.to_string(); }

    std::vector< game::space > chess_game::possible_moves( game::space const & src ) const
    {
        return game_board.possible_moves( src );
    }

}  // namespace chess