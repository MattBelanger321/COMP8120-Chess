#include <pawn.hpp>

namespace chess::pieces {
    pawn::pawn( bool const white, rank_t const rank, file_t const file ) : piece( white, name_t::pawn, rank, file ) {}

    std::vector< position_t > pawn::possible_moves() const
    {
        std::vector< position_t > moves;

        // pawn on the back rank
        if ( static_cast< int >( rank ) >= static_cast< int >( rank_t::h ) ||
             static_cast< int >( rank ) <= static_cast< int >( rank_t::a ) ) {
            // TODO: log error
            return {};
        }

        if ( is_white ) {
            if ( auto pos = itopos( static_cast< int >( rank ) + 1, static_cast< int >( file ) ); pos.has_value() ) {
                moves.push_back( *pos );
            }

            if ( auto pos = itopos( static_cast< int >( rank ) + 1, static_cast< int >( file ) + 1 );
                 pos.has_value() ) {
                moves.push_back( *pos );
            }

            if ( auto pos = itopos( static_cast< int >( rank ) + 1, static_cast< int >( file ) - 1 );
                 pos.has_value() ) {
                moves.push_back( *pos );
            }

            // first move
            if ( rank == rank_t::b ) {
                if ( auto pos = itopos( static_cast< int >( rank ) + 2, static_cast< int >( file ) );
                     pos.has_value() ) {
                    moves.push_back( *pos );
                }
            }
        }
        else {
            if ( auto pos = itopos( static_cast< int >( rank ) - 1, static_cast< int >( file ) ); pos.has_value() ) {
                moves.push_back( *pos );
            }

            if ( auto pos = itopos( static_cast< int >( rank ) - 1, static_cast< int >( file ) + 1 );
                 pos.has_value() ) {
                moves.push_back( *pos );
            }

            if ( auto pos = itopos( static_cast< int >( rank ) - 1, static_cast< int >( file ) - 1 );
                 pos.has_value() ) {
                moves.push_back( *pos );
            }

            // first move
            if ( rank == rank_t::g ) {
                if ( auto pos = itopos( static_cast< int >( rank ) - 2, static_cast< int >( file ) );
                     pos.has_value() ) {
                    moves.push_back( *pos );
                }
            }
        }

        return moves;
    }

}  // namespace chess::pieces