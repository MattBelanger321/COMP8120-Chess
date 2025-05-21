#ifndef __CHESS__PIECES__BISHOP__
#define __CHESS__PIECES__BISHOP__

#include <piece.hpp>

namespace chess::pieces {

    class bishop : public piece {
    public:
        bishop( bool const white, rank_t const rank, file_t const file );
        virtual std::vector< position_t > possible_moves() const override;
    };
}  // namespace chess::pieces

#endif