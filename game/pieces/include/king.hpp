#ifndef __CHESS__PIECES__KING__
#define __CHESS__PIECES__KING__

#include <piece.hpp>

namespace chess::pieces {

    class king : public piece {
    public:
        king( bool const white, rank_t const rank, file_t const file );
        virtual std::vector< position_t > possible_moves() const override;
    };
}  // namespace chess::pieces

#endif