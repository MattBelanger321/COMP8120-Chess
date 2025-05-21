#include <piece.hpp>
#include <stdexcept>
#include <string>

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

    position_t itopos( int const rank, int const file )
    {

        if ( rank > 8 ) {
            std::runtime_error( "rank too large: rank=" + std::to_string( rank ) );
        }

        if ( file > 8 ) {
            std::runtime_error( "file too large: file=" + std::to_string( file ) );
        }

        if ( rank < 1 ) {
            std::runtime_error( "rank too small: rank=" + std::to_string( rank ) );
        }

        if ( file < 1 ) {
            std::runtime_error( "file too small: file=" + std::to_string( file ) );
        }

        return { static_cast< rank_t >( rank ), static_cast< file_t >( file ) };
    }
}  // namespace chess::pieces
