#ifndef __CHESS__GAME__BOARD__
#define __CHESS__GAME__BOARD__

#include "piece.hpp"
#include <map>
#include <space.hpp>

namespace chess::game {

    using file_t = std::map< pieces::file_t, space >;   // cols
    using rank_t = std::map< pieces::rank_t, file_t >;  // rows

    class board {

    private:
        // NOTE: do to the nature of the entries of this map the [] operator will cause a compile error
        // please use the at() function
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

        // returns a reference to the space at the given position
        space const & get( pieces::position_t pos ) const;
    };
}  // namespace chess::game

#endif