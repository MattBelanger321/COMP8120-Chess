#include <rook.hpp>

namespace chess::pieces {
    rook::rook( bool const white, rank_t const rank, file_t const file ) : piece( white, name_t::rook, rank, file ) {}

    // NOTE: castling will be handeled by the game class and will be a king move
    std::vector< position_t > rook::possible_moves() const
    {
        std::vector< position_t > moves;

        for ( int i = -8; i <= 8; i++ ) {
            if ( i == 0 )
                continue;

            if ( auto pos = itopos( static_cast< int >( rank ) + i, static_cast< int >( file ) ); pos.has_value() ) {
                moves.push_back( *pos );
            }

            if ( auto pos = itopos( static_cast< int >( rank ), static_cast< int >( file ) + i ); pos.has_value() ) {
                moves.push_back( *pos );
            }
        }

        return moves;
    }

}  // namespace chess::pieces