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

    pieces::move_status chess_game::move( game::space const & src, game::space const & dst )
    {
        if ( !src.piece ) {
            return pieces::move_status::no_piece_to_move;
        }

        std::vector< game::space > pos;
        auto                       status = possible_moves( src, pos );

        if ( status != pieces::move_status::valid ) {
            return status;
        }

        if ( pos.empty() || std::find( pos.begin(), pos.end(), dst ) == pos.end() ) {
            return pieces::move_status::illegal_move;
        }

        status = game_board.move( src.position(), dst.position() );

        if ( status != pieces::move_status::valid ) {
            return status;
        }

        if ( white_move() ) {
            state = game_state::black_move;
        }
        else {
            state = game_state::white_move;
        }

        return pieces::move_status::valid;
    }

    bool chess_game::white_move() const { return state == game_state::white_move || state == game_state::white_check; }

    bool chess_game::black_move() const { return state == game_state::black_move || state == game_state::black_check; }

    game::space const & chess_game::get( pieces::position_t const & pos ) const { return game_board.get( pos ); }

    std::string chess_game::to_string() const { return game_board.to_string(); }

    pieces::move_status chess_game::possible_moves( game::space const &          src,
                                                    std::vector< game::space > & possible_moves ) const
    {
        if ( src.piece->colour() ) {
            if ( !white_move() ) {  // if it is not whites move than we cannot move a white piece
                return pieces::move_status::invalid_turn;
            }
        }
        else {
            if ( !black_move() ) {  // and vice versa
                return pieces::move_status::invalid_turn;
            }
        }

        possible_moves = game_board.possible_moves( src );

        return pieces::move_status::valid;
    }

}  // namespace chess