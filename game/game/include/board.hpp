#ifndef __CHESS__GAME__BOARD__
#define __CHESS__GAME__BOARD__

#include <map>
#include <space.hpp>

namespace chess::game {

    using file_t = std::map< pieces::file_t, space >;   // cols
    using rank_t = std::map< pieces::rank_t, file_t >;  // rows

    class board {

    private:
        rank_t game_board;

        // an init function to place all the pieces on the board
        void place_pieces();

    public:
        // empty is set if there should be no pieces put on the board
        board( bool const empty = false );

        std::vector< space > possible_moves( space const & src ) const;

        // TODO:
        // void move( space & src, space & dst );

        std::string to_string();
    };
}  // namespace chess::game

#endif