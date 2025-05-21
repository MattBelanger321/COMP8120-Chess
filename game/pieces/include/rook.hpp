#ifndef __CHESS__PIECES__ROOK__
#define __CHESS__PIECES__ROOK__

#include <piece.hpp>

namespace chess::pieces {

    class rook : public piece {
    public:
        rook( bool const white, rank_t const rank, file_t const file );
        virtual std::vector< position_t > possible_moves() const override;
    };
}  // namespace chess::pieces

#endif