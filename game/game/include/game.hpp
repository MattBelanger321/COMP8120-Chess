#ifndef __CHESS__GAME__
#define __CHESS__GAME__

#include <board.hpp>
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
        void move( game::space const & src, game::space const & dst );

    private:
        game_state  state;
        game::board game_board;

        // set if the king or rook has not moved yet
        bool king_side_castle;
        bool queen_side_castle;
    };
}  // namespace chess

#endif