#include <bishop.hpp>
#include <cstdlib>

namespace chess::pieces {
    bishop::bishop( bool const white, rank_t const rank, file_t const file ) :
        piece( white, name_t::bishop, rank, file )
    {
    }

    std::vector< position_t > bishop::possible_moves() const
    {
        std::vector< position_t > moves;

        for ( int i = -8; i <= 8; i++ ) {
            if ( i == 0 )
                continue;

            if ( auto pos = itopos( static_cast< int >( rank ) + i, static_cast< int >( file ) + i );
                 pos.has_value() ) {
                moves.push_back( *pos );
            }

            if ( auto pos = itopos( static_cast< int >( rank ) - i, static_cast< int >( file ) + i );
                 pos.has_value() ) {
                moves.push_back( *pos );
            }
        }

        return moves;
    }

}  // namespace chess::pieces