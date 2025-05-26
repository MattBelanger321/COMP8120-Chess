#ifndef __CHESS__GAME__
#define __CHESS__GAME__

#include <board.hpp>
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
    };

    class chess_game {
    public:
        chess_game();

        bool move( game::space const & src, game::space const & dst );

        game::space const & get( pieces::position_t const & pos ) const;

        std::string to_string() const;

        std::vector< game::space > possible_moves( game::space const & src ) const;

        bool white_move() const;
        bool black_move() const;

        void             start();
        void             stop();
        game_state const get_state() const;

    private:
        game_state  state;
        game::board game_board;

        // set if the king or rook has not moved yet
        bool king_side_castle_white;
        bool king_side_castle_black;
        bool queen_side_castle_white;
        bool queen_side_castle_black;
    };
}  // namespace chess

#endif