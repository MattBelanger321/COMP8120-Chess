#include "king.hpp"
#include "piece.hpp"
#include <algorithm>
#include <board.hpp>
#include <game.hpp>
#include <string>

namespace chess {

    chess_game::chess_game() :
        game_board(),
        white_king( *reinterpret_cast< pieces::king * >(
            game_board.get( pieces::piece::itopos( 1, 5 ).value() ).piece.get() ) ),
        black_king(
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 8, 5 ).value() ).piece.get() ) )
    {
        start();
    }

    void chess_game::start()
    {
        state = game_state::white_move;
        game_board.reset();

        white_king =
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 1, 5 ).value() ).piece.get() );
        black_king =
            *reinterpret_cast< pieces::king * >( game_board.get( pieces::piece::itopos( 8, 5 ).value() ).piece.get() );
    }

    bool chess_game::move( game::space const & src, game::space const & dst )
    {
        if ( !src.piece ) {
            return false;
        }

        auto const & pos = possible_moves( src );

        if ( pos.empty() || std::find( pos.begin(), pos.end(), dst ) == pos.end() ) {
            return false;
        }

        game_board.move( src.position(), dst.position() );

        if ( white_move() ) {
            state = game_state::black_move;
        }
        else {
            state = game_state::white_move;
        }

        return true;
    }

    bool chess_game::white_move() const { return state == game_state::white_move || state == game_state::white_check; }

    bool chess_game::black_move() const { return state == game_state::black_move || state == game_state::black_check; }

    game::space const & chess_game::get( pieces::position_t const & pos ) const { return game_board.get( pos ); }

    std::string chess_game::to_string() const { return game_board.to_string(); }

    std::vector< game::space > chess_game::possible_moves( game::space const & src ) const
    {

        if ( src.piece->colour() ) {
            if ( !white_move() ) {  // if it is not whites move than we cannot move a white piece
                return {};
            }
        }
        else {
            if ( !black_move() ) {  // and vice versa
                return {};
            }
        }

        return game_board.possible_moves( src );
    }

}  // namespace chess