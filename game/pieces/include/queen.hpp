#ifndef __CHESS__PIECES__QUEEN__
#define __CHESS__PIECES__QUEEN__

#include <piece.hpp>

namespace chess::pieces {

    class queen : public piece {
    public:
        queen( bool const white, rank_t const rank, file_t const file );
        virtual std::vector< position_t > possible_moves() const override;
    };
}  // namespace chess::pieces

#endif