#ifndef __CHESS__GAME__
#define __CHESS__GAME__

#include "king.hpp"
#include <board.hpp>
#include <memory>
#include <piece.hpp>
#include <space.hpp>

namespace chess {
    enum class game_state {
        white_move,
        black_move,
        white_check,
        black_check,
        white_wins,
        black_wins,
        white_offers_draw,
        black_offers_draw,
        white_resigns,
        black_resigns,
        draw,
        invalid_game_state,
    };

    inline std::string to_string( game_state state )
    {
        switch ( state ) {
        case game_state::white_move:
            return "White to move";
        case game_state::black_move:
            return "Black to move";
        case game_state::white_check:
            return "White is in check";
        case game_state::black_check:
            return "Black is in check";
        case game_state::white_wins:
            return "White wins";
        case game_state::black_wins:
            return "Black wins";
        case game_state::white_offers_draw:
            return "White offers a draw";
        case game_state::black_offers_draw:
            return "Black offers a draw";
        case game_state::white_resigns:
            return "White resigns";
        case game_state::black_resigns:
            return "Black resigns";
        case game_state::draw:
            return "Game is a draw";
        case game_state::invalid_game_state:
            return "Invalid game state";
        default:
            return "Unknown game state";
        }
    }

    class chess_game {
    public:
        chess_game();

        pieces::move_status move( game::space const & src, game::space const & dst );

        game::space const & get( pieces::position_t const & pos ) const;

        std::string to_string() const;

        pieces::move_status possible_moves( game::space const &          src,
                                            std::vector< game::space > & possible_moves ) const;

        bool white_move() const;
        bool black_move() const;

        void                    start();
        void                    stop();
        inline game_state const get_state() const { return state; }

        bool checkmate( bool const colour ) const;

    private:
        game_state  state;
        game::board game_board;

        // based on the moved piece thene functions update the castling status flags
        void white_castling_rights( std::unique_ptr< pieces::piece > const & moved_piece,
                                    pieces::position_t const &               src_pos );
        void black_castling_rights( std::unique_ptr< pieces::piece > const & moved_piece,
                                    pieces::position_t const &               src_pos );

        // set if the king or rook has not moved yet
        bool king_side_castle_white;
        bool king_side_castle_black;
        bool queen_side_castle_white;
        bool queen_side_castle_black;

        // read only references to the king so we can check for checks
        std::reference_wrapper< pieces::king const > white_king;
        std::reference_wrapper< pieces::king const > black_king;
    };
}  // namespace chess

#endif