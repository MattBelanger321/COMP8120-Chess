#include <king.hpp>

namespace chess::pieces {
    king::king( bool const white, rank_t const rank, file_t const file ) : piece( white, name_t::king, rank, file ) {}

    std::vector< position_t > king::possible_moves() const
    {
        std::vector< position_t > moves;
        return moves;
    }

}  // namespace chess::pieces