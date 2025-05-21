#ifndef __CHESS__PIECES__PAWN__
#define __CHESS__PIECES__PAWN__

#include <piece.hpp>

namespace chess::pieces {

    class pawn : public piece {
    public:
        pawn( bool const white, rank_t const rank, file_t const file );
        virtual std::vector< position_t > possible_moves() const override;
    };
}  // namespace chess::pieces

#endif