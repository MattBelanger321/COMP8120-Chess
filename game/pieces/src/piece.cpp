#include <piece.hpp>

namespace chess::pieces {

    piece::piece( bool const white, name_t const name, rank_t const rank, file_t const file )
    {
        is_white   = white;
        this->name = name;
        this->file = file;
        this->rank = rank;
    }

    bool       piece::colour() const { return is_white; }
    position_t piece::position() const { return { rank, file }; }
    name_t     piece::type() const { return name; }

    void piece::move( position_t const pos )
    {
        rank = pos.first;
        file = pos.second;
    }
}  // namespace chess::pieces
