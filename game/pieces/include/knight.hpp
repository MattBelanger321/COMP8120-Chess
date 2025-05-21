#ifndef __CHESS__PIECES__KNIGHT__
#define __CHESS__PIECES__KNIGHT__

#include <piece.hpp>

namespace chess::pieces {

    class knight : public piece {
    public:
        knight( bool const white, rank_t const rank, file_t const file );
        virtual std::vector< position_t > possible_moves() const override;
    };
}  // namespace chess::pieces

#endif